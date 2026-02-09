# Snake AI - Reinforcement Learning & Parallel Training

An advanced Snake game controlled by a Neural Network that learns to play through Reinforcement Learning. The project is designed for high-performance training, supporting parallel execution across Docker containers and Kubernetes clusters.

## 🧠 AI & Logic Architecture

The snake is powered by a custom Deep Q-Learning (DQN) inspired architecture. It doesn't just "see" the grid; it possesses a complex vision system consisting of **34 distinct inputs**:

*   **Raycasting (24 inputs):** Vision in 8 directions (N, NE, E, SE, S, SW, W, NW). For each direction, it senses the distance to walls, food, and its own body.
*   **Relative Food Sensing (2 inputs):** Precise X/Y vector to the food relative to the snake's current heading.
*   **Relative Tail Tracking (2 inputs):** Helps the snake learn "tail-following" strategies to manage space at extreme lengths.
*   **Survival Sensors (3 inputs):** Immediate danger detection for the block directly in front, to the left, and to the right.
*   **Space Awareness (3 inputs):** A real-time **Flood Fill** algorithm that calculates the percentage of accessible board space in each of the three possible move directions (Forward, Left, Right).

## 🛠 Tech Stack

*   **Language:** C++20
*   **Graphics:** SFML (Simple and Fast Multimedia Library)
*   **Build System:** CMake
*   **Containerization:** Docker (Multi-stage optimized builds)
*   **Orchestration:** Docker Compose & Kubernetes
*   **AI:** Custom Neural Network implementation (SimpleNN) with Atomic Model Persistence.

---

## 🚀 Getting Started

### 1. Local GUI Build (Evaluation Mode)
To watch the AI play on your computer:

```bash
# Install SFML (macOS example)
brew install sfml

# Build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .

# Run
cd SnakeAi
./SnakeAi
```

### 2. Docker Training (High-Speed Simulation)
To train the AI hundreds of times faster than real-time using Docker Compose:

```bash
# Build and start training
docker-compose up --build
```
*The model will automatically save to `model.txt` in your project folder every 10 attempts.*

### 3. Kubernetes Cluster (Parallel Training)
For professional-grade training, run multiple pods simultaneously that contribute to a single "Collective Intelligence":

```bash
# 1. Create the shared storage
kubectl apply -f pvc.yaml

# 2. Start the parallel training job
kubectl apply -f training-job.yaml

# 3. Monitor the progress
kubectl get pods -w
kubectl logs -f <pod-name>
```

---

## 📂 Project Structure

*   **/SnakeAi/Core**: The "Brain" and game logic.
    *   `AiAgent.cpp`: Vision processing, decision making, and model persistence.
    *   `SimpleNN.h`: Custom Neural Network implementation.
    *   `SnakeGame.cpp`: Core mechanics and BFS pathfinding.
*   **/SnakeAi**: UI and Visualization.
    *   `GameScene.cpp`: The graphical evaluation loop.
    *   `HeadlessTrainer.h`: The high-speed simulation loop.
*   `Dockerfile`: Multi-stage build for headless cloud execution.
*   `training-job.yaml`: Kubernetes configuration for parallelized learning.

## ⚖️ License
MIT License - Created by georgeen
