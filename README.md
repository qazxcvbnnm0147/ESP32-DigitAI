# ESP32-DigitAI

**ESP32-DigitAI** 是基於 ESP32-CAM 的嵌入式人工智慧專案，實現數字分割與識別，並通過 MQTT 協議進行即時資料傳輸，該專案展示了嵌入式 AI 與物聯網技術的結合。

## 功能介紹
- 使用 ESP32-CAM 捕捉影像並進行數字分割。
- 使用 TensorFlow Lite 模型進行數字識別，並對模型進行縮小及加速。
- 通過 MQTT 協議將識別結果上傳至伺服器或終端。
- 支援實時數據傳輸與處理

- ## 使用技術
- **硬體**：ESP32-CAM
- **軟體**：
  - TensorFlow Lite：模型訓練與推論
  - MQTT：數據傳輸協議
- **語言**：
  - C/C++：ESP32 上的核心程式
  - Python：用於模型訓練
 
- ## Demo 示範

### 硬體配置
![硬體配置](https://github.com/qazxcvbnnm0147/ESP32-DigitAI/raw/main/assets/hardware-setup.png)

### 數字識別過程
![數字識別](https://github.com/qazxcvbnnm0147/ESP32-DigitAI/raw/main/assets/digit-recognition.png)

### MQTT 數據顯示
![MQTT 數據](https://github.com/qazxcvbnnm0147/ESP32-DigitAI/raw/main/assets/mqtt-data-display.png)

# ESP32-DigitAI 專案結構

以下是本專案的目錄結構和每個部分的說明：

ESP32-DigitAI/ ├── esp32cam/ # ESP32-CAM 的相關代碼 │ ├── app_camera_esp.c # 相機應用邏輯 │ ├── detection_responder.cpp # 識別結果處理 │ ├── image_provider.cpp # 影像提供者 │ ├── model_data.cpp # TensorFlow Lite 模型數據 │ └── mutiple.ino # 主程序 ├── tensorflow/ # TensorFlow 模型相關文件 │ ├── mnist_model.tflite # 訓練後的壓縮模型 │ ├── tflit_trainning.py # 模型訓練腳本 └── README.md # 專案介紹文件

### 目錄結構說明：

- **`esp32cam/`**:
  - 包含了所有與 ESP32-CAM 開發相關的代碼。
  - **`app_camera_esp.c`**：負責相機應用邏輯，處理影像捕獲和前處理。
  - **`detection_responder.cpp`**：處理從 TensorFlow Lite 模型返回的識別結果，並進行後續操作。
  - **`image_provider.cpp`**：提供影像數據給 TensorFlow Lite 模型進行處理。
  - **`model_data.cpp`**：存儲 TensorFlow Lite 模型的相關數據。
  - **`mutiple.ino`**：ESP32-CAM 的主程序，控制硬體和執行相機相關的任務。

- **`tensorflow/`**:
  - 包含了與 TensorFlow 模型訓練和推理相關的文件。
  - **`mnist_model.tflite`**：訓練後的 TensorFlow Lite 模型，包含了識別手寫數字的能力。
  - **`tflit_trainning.py`**：用於訓練和量化模型的腳本，將 MNIST 數據集訓練後的模型轉換為 TensorFlow Lite 格式。
