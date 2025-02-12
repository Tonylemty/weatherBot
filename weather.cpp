#include <iostream>
#include <string>
#include <iomanip>
#include <ctime>
#include <string_view>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <curl/curl.h>
#include "nlohmann/json.hpp"

using namespace std;
using json = nlohmann::json;


const string USER_SETTINGS = "user_settings.json";

class Weather {
private:

    map<string, map<string, string>> translation = {

        {"en", {{"City", "City"}, {"Temperature", "Temperature"}, {"MaxTemp", "Max Temp"},
                {"MinTemp", "Min Temp"}, {"Pressure", "Pressure"}, {"Humidity", "Humidity"}}},
        {"zh_tw", {{"City", "城市"}, {"Temperature", "溫度"}, {"MaxTemp", "最高溫"},
                {"MinTemp", "最低溫"}, {"Pressure", "氣壓"}, {"Humidity", "濕度"}}},
        {"zh_cn", {{"City", "城市"}, {"Temperature", "温度"}, {"MaxTemp", "最高温"},
                {"MinTemp", "最低温"}, {"Pressure", "气压"}, {"Humidity", "湿度"}}},
        {"fr", {{"City", "Ville"}, {"Temperature", "Température"}, {"MaxTemp", "Temp max"},
                {"MinTemp", "Temp min"}, {"Pressure", "Pression"}, {"Humidity", "Humidité"}}},
        {"de", {{"City", "Stadt"}, {"Temperature", "Temperatur"}, {"MaxTemp", "Höchsttemp"},
                {"MinTemp", "Mindesttemp"}, {"Pressure", "Druck"}, {"Humidity", "Feuchtigkeit"}}},
        {"ja", {{"City", "都市"}, {"Temperature", "気温"}, {"MaxTemp", "最高気温"},
                {"MinTemp", "最低気温"}, {"Pressure", "気圧"}, {"Humidity", "湿度"}}},
        {"kr", {{"City", "도시"}, {"Temperature", "온도"}, {"MaxTemp", "최고 온도"},
                {"MinTemp", "최저 온도"}, {"Pressure", "기압"}, {"Humidity", "습도"}}},
        {"es", {{"City", "Ciudad"}, {"Temperature", "Temperatura"}, {"MaxTemp", "Temp. máx."},
                {"MinTemp", "Temp. mín."}, {"Pressure", "Presión"}, {"Humidity", "Humedad"}}},
        
    };
    
    string api_key;
    string url;
    string city_name;
    string weather_condition;
    string language_code;
    string temp_unit;

    int temp;
    int humidity;
    int pressure;
    int temp_max;
    int temp_min;

    // 處理 API 回應的函式
    static size_t WriteCallback(void* ptr, size_t size, size_t nmemb, std::string* data) {
        data->append((char*)ptr, size * nmemb);
        return size * nmemb;
    }

    // 獲取 API 數據
    string fetchData() {
        CURL* curl = curl_easy_init();
        string response;

        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);  // 允許 HTTP 轉向

            CURLcode res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                cerr << "CURL error: " << curl_easy_strerror(res) << endl;
            }

            curl_easy_cleanup(curl);
        }

        return response;
    }

    // 取得城市的經緯度
    json getCityCoordinates(const string& city, const string& api_key) {
        string geo_url = "http://api.openweathermap.org/geo/1.0/direct?q=" + city + "&limit=1&appid=" + api_key;
        CURL* curl = curl_easy_init();
        string response;

        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, geo_url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

            CURLcode res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                cerr << "CURL error: " << curl_easy_strerror(res) << endl;
            }

            curl_easy_cleanup(curl);
        }

        json data = json::parse(response);
        return data;
    }
public:
    map<string, map<string, string>> dict = {
        {"en", {
            {"enter_api", "Please enter your API key:"},
            {"select_unit", "Select a unit by number (1.°C 2.°F):"},
            {"enter_city", "Enter city name (or type 'history' to get history data):"},
            {"weather_history", "Weather History"},
            {"history_prompt", "How many pieces of data do you want (or type all):"},
            {"file_error", "Failed to open the file"},
            {"write_error", "Error opening file for writing"},
            {"save_prompt", "Do you want to save ? (y/n):"},
            {"invalid_json", "Invalid JSON structure"},
            {"api_error", "Error from API: "}
        }},
        {"zh_tw", {
            {"enter_api", "請輸入您的 API 金鑰："},
            {"select_unit", "請選擇一個單位（1.°C 2.°F）："},
            {"enter_city", "請輸入城市名稱（或輸入 'history' 以獲取歷史數據）："},
            {"weather_history", "天氣歷史"},
            {"history_prompt", "您想獲取多少條數據（或輸入 all）："},
            {"file_error", "無法打開文件"},
            {"write_error", "開啟文件寫入時出錯"},
            {"save_prompt", "您要保存嗎？(y/n):"},
            {"invalid_json", "無效的 JSON 結構"},
            {"api_error", "API 返回錯誤："}
        }},
        {"zh_cn", {
            {"enter_api", "请输入您的 API 密钥："},
            {"select_unit", "请选择一个单位（1.°C 2.°F）："},
            {"enter_city", "请输入城市名称（或输入 'history' 获取历史数据）："},
            {"weather_history", "天气历史"},
            {"history_prompt", "您想获取多少条数据（或输入 all）："},
            {"file_error", "无法打开文件"},
            {"write_error", "打开文件写入时出错"},
            {"save_prompt", "您要保存吗？(y/n):"},
            {"invalid_json", "无效的 JSON 结构"},
            {"api_error", "API 返回错误："}
        }},
        {"fr", {
            {"enter_api", "Entrez votre clé API:"},
            {"select_unit", "Sélectionnez une unité par numéro (1.°C 2.°F) :"},
            {"enter_city", "Entrez le nom de la ville (ou tapez 'history' pour obtenir les données historiques):"},
            {"weather_history", "Historique météo"},
            {"history_prompt", "Combien de données souhaitez-vous récupérer ? (ou tapez 'all') :"},
            {"file_error", "Impossible d’ouvrir le fichier"},
            {"write_error", "Erreur lors de l’ouverture du fichier en écriture"},
            {"save_prompt", "Voulez-vous enregistrer ? (y/n):"},
            {"invalid_json", "Structure JSON invalide"},
            {"api_error", "Erreur de l'API : "}
        }},
        {"de", {
            {"enter_api", "Geben Sie Ihren API-Schlüssel ein:"},
            {"select_unit", "Wählen Sie eine Einheit anhand der Nummer aus (1.°C 2.°F):"},
            {"enter_city", "Geben Sie den Stadtnamen ein (oder tippen Sie 'history', um Verlaufsdaten zu erhalten):"},
            {"weather_history", "Wetterverlauf"},
            {"history_prompt", "Wie viele Daten möchten Sie abrufen? (oder 'all' eingeben):"},
            {"file_error", "Datei konnte nicht geöffnet werden"},
            {"write_error", "Fehler beim Öffnen der Datei zum Schreiben"},
            {"save_prompt", "Möchten Sie speichern? (y/n):"},
            {"invalid_json", "Ungültige JSON-Struktur"},
            {"api_error", "Fehler von der API: "}
        }},
        {"ja", {
            {"enter_api", "APIキーを入力してください："},
            {"select_unit", "数字で単位を選択してください（1.°C 2.°F）："},
            {"enter_city", "都市名を入力してください（または 'history' と入力して履歴データを取得）："},
            {"weather_history", "天気履歴"},
            {"history_prompt", "取得したいデータ数を入力してください（または all と入力）："},
            {"file_error", "ファイルを開けませんでした"},
            {"write_error", "ファイルの書き込み時にエラーが発生しました"},
            {"save_prompt", "保存しますか？ (y/n):"},
            {"invalid_json", "無効な JSON 構造"},
            {"api_error", "API のエラー："}
        }},
        {"kr", {
            {"enter_api", "API 키를 입력하세요:"},
            {"select_unit", "숫자로 단위를 선택하세요 (1.°C 2.°F):"},
            {"enter_city", "도시 이름을 입력하세요 (‘history’를 입력하면 기록 데이터를 조회할 수 있습니다):"},
            {"weather_history", "날씨 기록"},
            {"history_prompt", "몇 개의 데이터를 가져오시겠습니까? (또는 'all' 입력):"},
            {"file_error", "파일을 열 수 없습니다"},
            {"write_error", "파일을 쓰는 중 오류가 발생했습니다"},
            {"save_prompt", "저장하시겠습니까? (y/n):"},
            {"invalid_json", "잘못된 JSON 구조"},
            {"api_error", "API 오류: "}
        }},
        {"es", {
            {"enter_api", "Introduzca su clave API:"},
            {"select_unit", "Seleccione una unidad por número (1.°C 2.°F):"},
            {"enter_city", "Ingrese el nombre de la ciudad (o escriba 'history' para obtener datos históricos):"},
            {"weather_history", "Historial del clima"},
            {"history_prompt", "¿Cuántos datos desea obtener? (o escriba 'all'): "},
            {"file_error", "No se pudo abrir el archivo"},
            {"write_error", "Error al abrir el archivo para escritura"},
            {"save_prompt", "¿Desea guardar? (y/n):"},
            {"invalid_json", "Estructura JSON no válida"},
            {"api_error", "Error de la API: "}
        }}
    };
public:
    Weather() {}

    Weather(const string &lang): language_code(lang) {}

    Weather(const string &key, const string &city, const string &lang, const string &unit): api_key(key), city_name(city), language_code(lang), temp_unit(unit) {

        json geo_data = getCityCoordinates(city_name, api_key);
        if (geo_data.is_array() && !geo_data.empty()) {
            double lat = geo_data.at(0).at("lat");
            double lon = geo_data.at(0).at("lon");
            url = "http://api.openweathermap.org/data/2.5/weather?lat=" + to_string(lat) + "&lon=" + to_string(lon) + "&appid=" + 
                  api_key + "&units=" + temp_unit + "&lang=" + language_code;

        } else {
            cerr << dict[language_code]["api_error"] << "City not found" << endl;
            exit(1);
        }
    }

    void run() {
        bool success = parseWeatherData();
        if (success) {
            outputData();
            saveHistory();
        }
    }

    bool parseWeatherData() {
        string jsonData = fetchData();
        json data = json::parse(jsonData);

        // json 原始碼
        // cout << "Raw json data: " << jsonData << endl;

        if (data.contains("message")) {
            std::cerr << dict[language_code]["api_error"] << data["message"] << std::endl;
            return false;
        }

        if (data.contains("weather") && data.contains("main") && data.contains("sys")) {
            weather_condition = data["weather"][0]["description"];
            temp = data["main"]["temp"];
            temp_max = data["main"]["temp_max"];
            temp_min = data["main"]["temp_min"];
            pressure = data["main"]["pressure"];
            humidity = data["main"]["humidity"];
            return true;
        } else {
            cout << dict[language_code]["invalid_json"] << endl;
        }
        return false;
    }

    string returnUnit(const string &u) {
        return (u == "metric" ? "°C" : "°F");
    }

    void saveHistory() {
        char save;
        cout << dict[language_code]["save_prompt"];
        cin >> save;

        if (save == 'y') {
            fstream file;
            time_t now = time(0);
            string current_time = ctime(&now);
            string u = returnUnit(temp_unit);

            file.open("weather_history.txt", ios::app);

            if (!file.is_open()) {
                cout << dict[language_code]["write_error"] << endl;
                return;
            }

            file << current_time;
            file << translation[language_code]["City"]<< ": " << city_name << endl;
            file << translation[language_code]["Temperature"]<< ": " << temp << u << endl;
            file << translation[language_code]["MaxTemp"]<< ": " << temp_max << u << endl;
            file << translation[language_code]["MinTemp"]<< ": " << temp_min << u << endl;
            file << translation[language_code]["Pressure"]<< ": " << pressure << "hPa" << endl;
            file << translation[language_code]["Humidity"]<< ": " << humidity << "%" << endl << endl;

            file.close();
        }

    }

    void getHistoryData(const string& num) {
        fstream file;
        file.open("weather_history.txt", ios::in);
    
        if (!file.is_open()) {
            cout << dict[language_code]["file_error"] << endl;
            return;
        }
    
        string line;
        cout << "========= " << dict[language_code]["weather_history"] << " =========" << endl;
    
        if (num == "all") {
            while (getline(file, line)) {
                cout << line << endl;
            }
        } else {
            int count = 0;
            int targetCount = stoi(num);
            while (count < targetCount && getline(file, line)) {
                cout << line << endl;
                if (line.empty()) {
                    count++;
                }
            }
        }
    
        file.close();
    }

    void outputData() {
        string u = returnUnit(temp_unit);

        cout << translation[language_code]["City"]<< ": " << city_name << endl;
        cout << translation[language_code]["Temperature"]<< ": " << temp << u << endl;
        cout << translation[language_code]["MaxTemp"]<< ": " << temp_max << u << endl;
        cout << translation[language_code]["MinTemp"]<< ": " << temp_min << u << endl;
        cout << translation[language_code]["Pressure"]<< ": " << pressure << "hPa" << endl;
        cout << translation[language_code]["Humidity"]<< ": " << humidity << "%" << endl << endl;
        
    }
};

json readSettings() {
    fstream inputFile(USER_SETTINGS, ios::in);
    json config;

    if (inputFile.is_open()) {
        inputFile >> config;
        inputFile.close();
    } else {
        config = {
            {"isFirstTime", true},
            {"api_key", ""},
            {"languages", "en"},
            {"unit", "metric"}
        };
    }
    return config;
}

void writeSettings(const json &config) {
    fstream outputFile(USER_SETTINGS);

    if (outputFile.is_open()) {
        outputFile << config.dump(4);
        outputFile.close();
    } else {
        cerr << "Unable to write to JSON file" << endl;
    }
}

bool isFirstTime(const json &config) {
    return config["isFirstTime"].get<bool>();
}

int main(int argc, char* argv[]) {
    
    json settings = readSettings();

    // 如果提供了設定參數，則更新設定
    if (argc >= 5 && string(argv[1]) == "setconfig") {
        string api_key = argv[2];
        string lang = argv[3];
        string unit = argv[4];

        settings["api_key"] = api_key;
        settings["languages"] = lang;
        settings["unit"] = unit;
        settings["isFirstTime"] = false;

        writeSettings(settings);
        cout << "Configuration updated successfully." << endl;
        return 0;
    }

    // 正常執行天氣查詢
    if (argc < 4) {
        cerr << "Usage: weather <city> <language> <unit>" << endl;
        return 1;
    }

    string city = argv[1];
    string selected_lang = argv[2];
    string selected_unit = argv[3];
    string API_KEY = settings["api_key"].get<string>();

    if (city == "history") {
        string num = (argc > 4) ? argv[4] : "all";
        Weather getHistory(selected_lang);
        getHistory.getHistoryData(num);
    } else {
        Weather Client(API_KEY, city, selected_lang, selected_unit);
        Client.run();
    }

    return 0;
}