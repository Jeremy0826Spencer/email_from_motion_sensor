#include <WiFi.h>
#include <ESP_Mail_Client.h>

#define WIFI_SSID "name of wifi to connect to"
#define WIFI_PASSWORD "wifi password"
#define SMTP_SERVER "smtp.gmail.com" //if gmail
#define SMTP_PORT 465
#define SENDER_EMAIL "senderemail@email.com"
#define SENDER_PASSWORD "you must get this password from your gmail by setting up an app password"  // Replace with your app-specific password
#define RECIPIENT_EMAIL "receiveremail@email.com"
#define RECIPIENT_NAME "Jer"

#define PIR_PIN 4  // GPIO pin where HC-SR501 PIR sensor is connected

SMTPSession smtp;
bool motionDetected = false;  // Flag to track motion detection

// Function to configure SMTP session and send email
void sendEmail() {
  // Configure SMTP session
  ESP_Mail_Session session;
  session.server.host_name = SMTP_SERVER;
  session.server.port = SMTP_PORT;
  session.login.email = SENDER_EMAIL;
  session.login.password = SENDER_PASSWORD;
  session.login.user_domain = "";

  // Configure email message
  SMTP_Message message;
  message.sender.name = "ESP32";
  message.sender.email = SENDER_EMAIL;
  message.subject = "ESP32 Motion Detected!";
  message.addRecipient(RECIPIENT_NAME, RECIPIENT_EMAIL);

  // HTML message content
  String htmlMsg = "<div style=\"color:#000000;\"><h1>Motion Detected!</h1><p>Motion has been detected by the ESP32 using the PIR sensor.</p></div>";
  message.html.content = htmlMsg.c_str();
  message.html.charSet = "us-ascii";
  message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

  // Plain text message content
  String textMsg = "Motion has been detected by the ESP32 using the PIR sensor.";
  message.text.content = textMsg.c_str();
  message.text.charSet = "us-ascii";
  message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

  // Connect and send the email
  if (!smtp.connect(&session)) {
    Serial.println("Failed to connect to SMTP server");
    return;
  }

  if (!MailClient.sendMail(&smtp, &message)) {
    Serial.print("Error sending Email: ");
    Serial.println(smtp.errorReason());
  } else {
    Serial.println("Email sent successfully!");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Setup started.");

  pinMode(PIR_PIN, INPUT_PULLDOWN);  // Set PIR pin as input with pull-down resistor

  Serial.print("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // Attempt to connect to WiFi network
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("\nWiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  smtp.debug(1);
  Serial.println("Setup completed.");
}

void loop() {
  Serial.println("Loop started.");

  // Check if motion is detected by the PIR sensor
  int pirValue = digitalRead(PIR_PIN);
  
  if (pirValue == HIGH && !motionDetected) {
    Serial.println("Motion detected! Sending email...");
    sendEmail();
    motionDetected = true;  // Set the flag to indicate that motion is detected
  }

  // Reset the flag when motion stops
  if (pirValue == LOW && motionDetected) {
    Serial.println("Motion stopped.");
    motionDetected = false;  // Reset the flag to allow for next detection
  }

  delay(500);  // Short delay to avoid printing too much
}
