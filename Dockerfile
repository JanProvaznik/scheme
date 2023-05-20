FROM gcc:latest
COPY . /app
RUN apt-get update && apt-get install -y cmake
WORKDIR /app
RUN mkdir "build"
WORKDIR /app/build
RUN cmake ..
RUN make
CMD ["echo", "run ./Scheme for repl or ./Scheme <filename>"]
CMD ["/bin/bash"]
