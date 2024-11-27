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
![硬體配置](https://github.com/qazxcvbnnm0147/ESP32-DigitAI/raw/main/assets/hardware-setup.jpg)

### 數字識別過程
![數字識別](https://github.com/qazxcvbnnm0147/ESP32-DigitAI/raw/main/assets/digit-recognition.gif)

### MQTT 數據顯示
![MQTT 數據](https://github.com/qazxcvbnnm0147/ESP32-DigitAI/raw/main/assets/mqtt-data-display.jpg)
