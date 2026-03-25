#include <iostream>
#include <chrono>
#include <vector>
#include "ECS.h"

// Define test components (must be compatible with original ECS)
struct Position { float x, y; };
struct Velocity { float vx, vy; };

void run_benchmark() {
    const int ENTITY_COUNT = 10000;
    const int ITERATIONS = 1000;
    
    EntityManager em;
    
    // Setup: Create entities with mixed components
    // 50% have Position + Velocity, 50% have only Position
    for (int i = 0; i < ENTITY_COUNT; ++i) {
        EntityID e = em.createEntity();
        em.addComponent(e, Position{1.0f, 1.0f});
        if (i % 2 == 0) {
            em.addComponent(e, Velocity{0.5f, 0.5f});
        }
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Simulation: Update positions based on velocity
    // This mimics a typical "MovementSystem" update loop
    for (int iter = 0; iter < ITERATIONS; ++iter) {
        // In the original ECS, we must iterate over one type and check for the other
        // This triggers the map lookup bottleneck inside the inner loop
        auto& allPositions = const_cast<std::vector<std::pair<EntityID, Position>>&>(em.getAllComponents<Position>());
        
        for (auto& pair : allPositions) {
            EntityID e = pair.first;
            Position* pos = &pair.second;
            
            // Bottleneck: em.getComponent<T> performs a map lookup every time
            Velocity* vel = em.getComponent<Velocity>(e);
            if (vel) {
                pos->x += vel->vx;
                pos->y += vel->vy;
            }
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    
    std::cout << "--- ECS Performance Baseline ---" << std::endl;
    std::cout << "Entities: " << ENTITY_COUNT << " (5,000 active)" << std::endl;
    std::cout << "Iterations: " << ITERATIONS << std::endl;
    std::cout << "Total Time: " << duration << "ms" << std::endl;
    std::cout << "-------------------------------" << std::endl;
}

int main() {
    run_benchmark();
    return 0;
}
