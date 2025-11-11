Program ini berfungsi untuk:

* Mengukur **suhu air / udara** menggunakan **sensor DS18B20**.
* Menampilkan hasilnya di **LCD 16x2 I2C** dan **Serial Monitor**.
* Menghubungkan NodeMCU ke **WiFi**, kemudian membuat **Web Server lokal** (dapat diakses lewat IP di browser).
* Dari halaman web itu, kamu bisa **menyalakan (ON)** atau **mematikan (OFF)** **relay** dengan tombol.
* **Buzzer** berbunyi secara berkala sebagai penanda sistem aktif.

---

## ⚙️ **2. Komponen yang Digunakan**

| Komponen                  | Fungsi                                                  |
| ------------------------- | ------------------------------------------------------- |
| NodeMCU ESP8266           | Mikrokontroler + WiFi                                   |
| Sensor DS18B20            | Mengukur suhu                                           |
| LCD I2C 16x2              | Menampilkan hasil pengukuran suhu dan status            |
| Relay                     | Mengontrol perangkat (misal: kipas, heater, pompa, dll) |
| Buzzer                    | Alarm indikator                                         |
| Breadboard + kabel jumper | Koneksi rangkaian                                       |

---

## 🔌 **3. Wiring / Skema Koneksi**

### 🟦 **NodeMCU ke Sensor DS18B20**

| DS18B20 Pin    | NodeMCU Pin         | Keterangan             |
| -------------- | ------------------- | ---------------------- |
| VCC            | 3.3V                | Daya                   |
| GND            | GND                 | Ground                 |
| DATA           | D3 (GPIO0)          | Jalur data 1-Wire      |
| Resistor 4.7kΩ | Antara DATA dan VCC | Pull-up resistor wajib |

---

### 🟩 **NodeMCU ke LCD I2C**

| LCD I2C Pin | NodeMCU Pin | Keterangan      |
| ----------- | ----------- | --------------- |
| VCC         | 5V          | Daya LCD        |
| GND         | GND         | Ground          |
| SDA         | D2 (GPIO4)  | Jalur data I2C  |
| SCL         | D1 (GPIO5)  | Jalur clock I2C |

---

### 🟧 **NodeMCU ke Relay**

| Relay Pin | NodeMCU Pin | Keterangan       |
| --------- | ----------- | ---------------- |
| IN        | D5 (GPIO14) | Sinyal kontrol   |
| VCC       | 5V          | Daya modul relay |
| GND       | GND         | Ground           |

> ⚠️ Jika modul relay kamu 5V, pastikan pakai **power supply eksternal 5V**, dan **hubungkan GND bersama dengan NodeMCU**.

---

### 🟥 **NodeMCU ke Buzzer**

| Buzzer Pin | NodeMCU Pin | Keterangan    |
| ---------- | ----------- | ------------- |
| +          | D6 (GPIO12) | Kontrol suara |
| -          | GND         | Ground        |

---

## 🧩 **4. Penjelasan Alur Program**

### 🔹 **Inisialisasi Komponen**

```cpp
LiquidCrystal_I2C lcd(0x27, 16, 2);
OneWire oneWire(0);
DallasTemperature sensors(&oneWire);
```

* Membuat objek LCD dan sensor suhu.
* `OneWire(0)` artinya sensor DS18B20 terhubung di pin **GPIO0 (D3)**.

---

### 🔹 **Koneksi WiFi**

```cpp
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print(".");
}
```

NodeMCU akan mencoba tersambung ke WiFi dengan SSID `"aku"` dan password `"12345678910"`.
Jika berhasil, alamat IP-nya akan ditampilkan di LCD dan Serial Monitor.

---

### 🔹 **Menjalankan Web Server**

```cpp
WiFiServer server(80);
server.begin();
```

* Membuka server lokal pada port 80.
* Server ini bisa diakses di browser HP/laptop dengan mengetik **IP address NodeMCU** (contoh: `192.168.1.10`).

---

### 🔹 **Menampilkan Suhu di LCD**

```cpp
sensors.requestTemperatures();
float tempC = sensors.getTempCByIndex(0);
lcd.print("SUHU:");
lcd.print(tempC);
lcd.print("°C");
```

* Setiap 1 detik, NodeMCU membaca suhu dari sensor DS18B20.
* Hasil ditampilkan di LCD dan Serial Monitor.

---

### 🔹 **Buzzer sebagai Indikator**

```cpp
digitalWrite(buzzerPin, HIGH);
delay(500);
digitalWrite(buzzerPin, LOW);
```

* Buzzer berbunyi singkat setiap loop untuk memastikan sistem hidup.

---

### 🔹 **Kontrol Relay dari Web Server**

NodeMCU membaca permintaan HTTP:

```cpp
if (currentLine.endsWith("GET /H")) digitalWrite(relayPin, HIGH); // ON
if (currentLine.endsWith("GET /L")) digitalWrite(relayPin, LOW);  // OFF
```

Kemudian menampilkan halaman sederhana:

```cpp
client.print("Tekan <a href=\"/H\">Tombol</a> untuk menhidupkan relay.<br>");
client.print("Tekan <a href=\"/L\">Tombol</a> untuk mematikan relay.<br>");
```

Jadi ketika kamu membuka IP NodeMCU di browser:

```
http://192.168.1.xx
```

Akan muncul dua link:

* Klik **Tombol pertama (H)** → Relay ON
* Klik **Tombol kedua (L)** → Relay OFF

---

## 🖥️ **5. Tampilan Web di Browser**

Contohnya seperti ini:

```
Tekan Tombol untuk menhidupkan relay.
Tekan Tombol untuk mematikan relay.
```

Klik “Tombol” pertama → relay menyala, klik “Tombol” kedua → relay mati.

---

## 📶 **6. Ringkasan Alur Kerja**

➡️ NodeMCU tersambung ke WiFi
➡️ Membaca suhu dari DS18B20
➡️ Menampilkan suhu di LCD
➡️ Membunyikan buzzer secara periodik
➡️ Menyediakan halaman web kontrol (via IP Address)
➡️ Pengguna bisa ON/OFF relay dari HP/laptop lewat browser

