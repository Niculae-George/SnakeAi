# Stage 1: Build
FROM ubuntu:22.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

# Build Headless version
RUN rm -rf build && \
    cmake -B build -S . -DBUILD_HEADLESS=ON -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build

# Stage 2: Runtime
FROM ubuntu:22.04

# Install basic runtime dependencies (for libstdc++, etc)
RUN apt-get update && apt-get install -y \
    libstdc++6 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy the headless binary
COPY --from=builder /app/build/SnakeAi/SnakeAiHeadless .

# Copy shared libraries and update loader cache
COPY --from=builder /app/build/_deps/sfml-build/lib/libsfml-system.so* /usr/local/lib/
RUN ldconfig

# Default to headless training (10,000 attempts)
ENTRYPOINT ["./SnakeAiHeadless", "--headless"]
CMD ["10000"]