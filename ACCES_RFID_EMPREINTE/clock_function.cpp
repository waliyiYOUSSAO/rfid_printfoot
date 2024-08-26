#include "clock_function.h"

void Clock_::Begin() {
    rtc.Begin();
}

// void Clock_::init_WiFi(const char* ssid, const char* password) {
//     WiFi.mode(WIFI_STA);
//     WiFi.begin(ssid, password);
//     Serial.print("Connexion au WiFi...");
//     while (WiFi.status() != WL_CONNECTED) {
//         Serial.print('.');
//         delay(500);
//     }
//     Serial.println(WiFi.localIP());
// }

void Clock_::sync_rtc() {
    const char* ntp_server = "pool.ntp.org"; // Serveur NTP pour obtenir l'heure
    const long gmt_offset_sec = 3600; // Décalage horaire en secondes
    const int day_light_offset_sec = 0; // Décalage d'été au Bénin

    configTime(gmt_offset_sec, day_light_offset_sec, ntp_server);
    struct tm time_info;
    if (!getLocalTime(&time_info)) {
        Serial.println("Échec de l'obtention de l'heure");
        return;
    }

    RtcDateTime now = RtcDateTime(time_info.tm_year + 1900, time_info.tm_mon + 1, time_info.tm_mday, 
                                  time_info.tm_hour, time_info.tm_min, time_info.tm_sec);

    rtc.SetDateTime(now);
}

String Clock_::show_date() {
    char days_of_week[7][5] = { "Dim","Lun", "Mar", "Mer", "Jeu", "Ven", "Sam"}; // Jours de la semaine
    RtcDateTime now = rtc.GetDateTime();
    String date = "";
    //String date = String(days_of_week[now.DayOfWeek()]) + ",";
    date += (now.Day() < 10 ? "0" : "") + String(now.Day()) + "/";
    date += (now.Month() < 10 ? "0" : "") + String(now.Month()) + "/";
    date += String(now.Year()) + ",";
    date += (now.Hour() < 10 ? "0" : "") + String(now.Hour()) + ":";
    date += (now.Minute() < 10 ? "0" : "") + String(now.Minute()) + ":";
    date += (now.Second() < 10 ? "0" : "") + String(now.Second());

    return date;
}
