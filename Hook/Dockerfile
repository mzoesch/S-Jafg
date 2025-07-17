# Copyright mzoesch. All rights reserved.

FROM python:3.11-slim

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    curl \
    build-essential \
    gcc \
    g++ \
    pkg-config \
    libssl-dev \
    ca-certificates \
    git \
    wget \
    mingw-w64 \
    && rm -rf /var/lib/apt/lists/*

ENV RUSTUP_HOME=/usr/local/rustup \
    CARGO_HOME=/usr/local/cargo \
    PATH=/usr/local/cargo/bin:$PATH

RUN curl https://sh.rustup.rs -sSf | sh -s -- -y --default-toolchain stable
RUN rustup target add x86_64-pc-windows-gnu

WORKDIR /app

COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

COPY . .

RUN echo "alias ll='ls -las'" >> ~/.bashrc

EXPOSE 8000
CMD ["uvicorn", "Launch:app", "--host", "0.0.0.0", "--port", "8000"]
