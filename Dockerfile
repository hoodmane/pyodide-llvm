FROM node:14.16.1-buster-slim
RUN apt-get update \
  && apt-get install -y --no-install-recommends \ 
    lsb-release wget software-properties-common \
    git \
    gnupg2 cmake \
    build-essential \
  && wget https://apt.llvm.org/llvm.sh \
  && chmod +x llvm.sh \
  && ./llvm.sh 13 \
  && apt-get install -y libclang-13-dev lld-13 nodejs zlib1g-dev \
  && rm -rf /var/lib/apts/lists/*

ENV PATH="/usr/lib/llvm-13/bin:/src/.docker_home/.local/bin:${PATH}"
WORKDIR /src
