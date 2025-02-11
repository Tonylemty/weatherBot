apt update && apt install -y nlohmann-json3-dev
g++ weather.cpp -o weather -lcurl -std=c++17
chmod +x weather