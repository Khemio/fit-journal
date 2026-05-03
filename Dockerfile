# Stage 1: Build stage
FROM drogonframework/drogon:latest AS build

# Set the working directory
WORKDIR /app

# Copy the source code into the container
COPY src/ ./src
COPY static/ ./static
COPY vendor/ ./vendor
COPY CMakeLists.txt .
COPY config.json .

# Compile the C++ code statically to ensure it doesn't depend on runtime libraries
RUN mkdir build && cd build && mkdir logs; \
    cmake .. && cmake --build .; \
    cp -r ../src/views build/
    # cd .. ; \
    # mkdir logs && mv config.json ..
    # mv journal static/ views_gen/ ..; \
    # cd .. && rm -rf build 

WORKDIR /app/build

# Expose ports
EXPOSE 5555

ENTRYPOINT ["./journal"]