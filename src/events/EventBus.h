#pragma once

#include <cstdint>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>
#include <deque>
#include <memory>
#include <functional>
#include <mutex>
#include <atomic>
#include <utility>

/*
	IEvent: Base interface for all events.
	Provides type-erasure for heterogeneous event queueing.
*/
class IEvent
{
public:
	virtual ~IEvent() = default;

	// Returns runtime type index for subscriber dispatch
	virtual std::type_index type() const noexcept = 0;
};

/*
	Event<T>: Type-safe event wrapper carrying payload of type T.
*/
template<typename T>
class Event : public IEvent
{
public:
	// Construct from lvalue/rvalue - perfect forwarding
	template<typename U>
	explicit Event(U&& data) : m_data(std::forward<U>(data)) {}

	// Copy/move from Event
	Event(const Event&) = default;
	Event(Event&&) noexcept = default;
	Event& operator=(const Event&) = default;
	Event& operator=(Event&&) noexcept = default;

	std::type_index type() const noexcept override
	{
		return std::type_index(typeid(T));
	}

	// Access payload
	const T& data() const noexcept { return m_data; }
	T& data() noexcept { return m_data; }

private:
	T m_data;
};

/*
	EventBus: Thread-safe observer pattern implementation.
	
	Features:
	- Template-based subscribe<T>(callback) for type-safe registration
	- publish<T>(data) queues events without immediate dispatch
	- dispatch() delivers all queued events to subscribers
	- No data loss: queue is atomic; subscribers receive all events
	- Thread-safe: mutex guards all shared state
	- Re-entrant dispatch protection via atomic flag
*/
class EventBus
{
public:
	// Callback signature: receives const reference to base event
	// Concrete handlers cast via Event<T>::data()
	using Callback = std::function<void(const IEvent&)>;

	EventBus() = default;
	~EventBus() = default;

	// Non-copyable, non-movable (owns mutex state)
	EventBus(const EventBus&) = delete;
	EventBus& operator=(const EventBus&) = delete;
	EventBus(EventBus&&) = delete;
	EventBus& operator=(EventBus&&) = delete;

	/*
		Subscribe to event type T.
		Callback receives const Event<T>& via type-erased IEvent.
		Returns subscription ID for potential unsubscription (future).
	*/
	template<typename T>
	void subscribe(Callback callback)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		const std::type_index key(typeid(T));
		m_subscribers[key].push_back(std::move(callback));
	}

	/*
		Subscribe with member function pointer.
		Example: bus.subscribe<MatchEvent>(&handler, &Handler::onMatch);
	*/
	template<typename T, typename Class>
	void subscribe(Class* instance, void (Class::*method)(const Event<T>&))
	{
		subscribe<T>([instance, method](const IEvent& e) {
			(instance->*method)(static_cast<const Event<T>&>(e));
		});
	}

	/*
		Publish event of type T with payload.
		Queues event for later dispatch (decoupled delivery).
		Thread-safe: safe to call from any thread.
	*/
	template<typename T>
	void publish(T&& eventData)
	{
		auto event = std::make_unique<Event<std::decay_t<T>>>(std::forward<T>(eventData));
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_queue.push_back(std::move(event));
		}
	}

	/*
		Publish pre-constructed event (for custom Event subclasses).
		Takes ownership via unique_ptr.
	*/
	void publish(std::unique_ptr<IEvent> event)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_queue.push_back(std::move(event));
	}

	/*
		Dispatch all queued events to subscribers.
		- Thread-safe: mutex guards queue and subscriber access
		- No data loss: queue drained atomically per batch
		- Re-entrant safe: atomic flag prevents nested dispatch
		- Exception safe: one subscriber failure doesn't drop others
	*/
	void dispatch()
	{
		// Guard against re-entrant dispatch
		if (m_dispatching.exchange(true, std::memory_order_acquire))
		{
			return; // Already dispatching; events will be processed
		}

		// Drain queue under lock, then process outside lock for callbacks
		std::deque<std::unique_ptr<IEvent>> localQueue;

		{
			std::lock_guard<std::mutex> lock(m_mutex);
			localQueue.swap(m_queue);
		}

		// Process outside lock to avoid holding mutex during callbacks
		while (!localQueue.empty())
		{
			auto& event = localQueue.front();
			const std::type_index key = event->type();

			// Copy subscriber list under lock
			std::vector<Callback> handlers;
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				auto it = m_subscribers.find(key);
				if (it != m_subscribers.end())
				{
					handlers = it->second;
				}
			}

			// Invoke all handlers for this event type
			for (const auto& cb : handlers)
			{
				try
				{
					cb(*event);
				}
				catch (...)
				{
					// Swallow callback exceptions to prevent data loss
					// Future: could log or forward to error handler
				}
			}

			localQueue.pop_front();
		}

		m_dispatching.store(false, std::memory_order_release);
	}

	/*
		Clear all queued events without dispatching.
		Useful for shutdown or state reset.
	*/
	void clearQueue()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_queue.clear();
	}

	/*
		Clear all subscribers.
		Useful for shutdown or test teardown.
	*/
	void clearSubscribers()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_subscribers.clear();
	}

	/*
		Check if queue has pending events.
	*/
	bool hasPending() const
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return !m_queue.empty();
	}

private:
	// Per-type subscriber lists
	std::unordered_map<std::type_index, std::vector<Callback>> m_subscribers;

	// Event queue (FIFO)
	std::deque<std::unique_ptr<IEvent>> m_queue;

	// Protects subscribers and queue
	mutable std::mutex m_mutex;

	// Re-entrant dispatch guard
	std::atomic<bool> m_dispatching{false};
};
