# rfid_printfoot_blolab

RFID Access Control System
This repository contains the code for an RFID-based access control system using an ESP microcontroller. The system is designed to manage entry and exit access for employees by reading RFID cards, storing user information, and logging access history.

Features
RFID Card Reading: Reads RFID cards to identify users.
File Serving: Serves HTML files from the ESP's filesystem for web-based interfaces.
User Authentication: Handles login authentication for accessing the admin panel.
Card Information Storage: Saves user details (RFID ID, name, department) locally.
In/Out Tracking: Tracks whether a user is entering or exiting based on RFID scans.
Data Transmission: Sends data to Google Sheets for logging access history.
User Information Retrieval: Retrieves and displays user details based on RFID scans.
File Operations: Supports reading, writing, and deleting files on the ESP's filesystem.
Functions
String rfid_read()
Reads the RFID card's unique identifier and returns it as a string.

void serve_file(const char* path)
Serves a file from the ESP's filesystem to the client, primarily used for serving HTML pages.

void handle_login_form()
Handles the login form submission, verifying the username and password and redirecting to the admin panel if successful.

String wait_for_rfid()
Waits for an RFID card to be scanned, with a timeout of 5 seconds, and returns the card ID.

void save_card_info(String card_id, String first_name, String last_name, String department)
Saves the user information (RFID card ID, first name, last name, and department) to a file if the card ID is not already registered.

void readFile(String path)
Reads and prints the contents of a file, used to list registered employees.

bool unicity(String path, String card_id)
Checks if an RFID card ID already exists in the specified file to ensure uniqueness.

int in_or_out(String in_out_id, String path)
Determines whether a user is entering or exiting and updates the in/out status in the file.

void display_some(String sentence_1, String sentence_2)
Displays a message on the LCD screen, typically used for showing entry or exit status.

void send_data(String sheet_name, String rfid_string, String first_name, String statut, String hour)
Sends user data to a specified Google Sheet for logging purposes.

String name_after_verification(String path, String card)
Verifies the RFID card ID and returns the associated first name.

Last_name_department_sheet extract_last_name_and_department(String path, String card)
Extracts the last name and department associated with an RFID card ID from the specified file.

void deleteFile(String path)
Deletes a specified file from the ESP's filesystem.

void entered_exit_rfid()
Handles the entry/exit process by reading the RFID card, verifying access, and updating the in/out status.

Getting Started
Clone the repository:
bash
Copier le code
git clone https://github.com/yourusername/your-repo-name.git
Upload the code to your ESP microcontroller.
Ensure the necessary libraries (e.g., LittleFS, MFRC522, ESP8266WiFi, HTTPSRedirect) are installed.
Customize the code for your environment, such as setting the correct Wi-Fi credentials and Google Sheets API details.