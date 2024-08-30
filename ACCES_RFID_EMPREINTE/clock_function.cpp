// #include "clock_function.h"

// void Clock_::Begin() {
//     if(!rtc.Begin()){
//       Serial.println("RTC initialization failed!");
//       while (1);
//     }
// }



// // void Clock_::sync_rtc() {
// //     const char* ntp_server = "pool.ntp.org"; // Serveur NTP pour obtenir l'heure
// //     const long gmt_offset_sec = 3600; // Décalage horaire en secondes
// //     const int day_light_offset_sec = 0; // Décalage d'été au Bénin

// //     configTime(gmt_offset_sec, day_light_offset_sec, ntp_server);
// //     struct tm time_info;
// //     if (!getLocalTime(&time_info)) {
// //         Serial.println("Échec de l'obtention de l'heure");
// //         return;
// //     }

// //     // RtcDateTime now = RtcDateTime(time_info.tm_year + 1900, time_info.tm_mon + 1, time_info.tm_mday, 
// //     //                               time_info.tm_hour, time_info.tm_min, time_info.tm_sec);
// //     RtcDateTime now = RtcDateTime(124 + 1900, 5 + 1, 5, 12, 30, 0);

// //     rtc.SetDateTime(now);
// // }

// void Clock_::set_manual_time(int year, int month, int day, int hour, int minute, int second) {
//     RtcDateTime now = RtcDateTime(year, month, day, hour, minute, second);
//     rtc.SetDateTime(now);
// }


// String Clock_::show_date() {
//     char days_of_week[7][5] = { "Dim","Lun", "Mar", "Mer", "Jeu", "Ven", "Sam"}; // Jours de la semaine
//     RtcDateTime now = rtc.GetDateTime();
//     String date = "";
//     //String date = String(days_of_week[now.DayOfWeek()]) + ",";
//     date += (now.Day() < 10 ? "0" : "") + String(now.Day()) + "/";
//     date += (now.Month() < 10 ? "0" : "") + String(now.Month()) + "/";
//     date += String(now.Year()) + ",";
//     date += (now.Hour() < 10 ? "0" : "") + String(now.Hour()) + ":";
//     date += (now.Minute() < 10 ? "0" : "") + String(now.Minute()) + ":";
//     date += (now.Second() < 10 ? "0" : "") + String(now.Second());
//     return date;
// }
