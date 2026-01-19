#include "Logger.h"
#include <stdio.h>
/*
Logger.c - Implementering av loggningsfunktionen 
Skriver felmeddelandet, vilken modul felet uppstod i, skriver till fil med tidsstämpel och loggar till server_log.txt.

Exempelanropp:
log_message("MAIN", "Programmet startade", "INFO", now);
ger följande rad i loggfilen:
[2024-06-15 14:23:01] [INFO] [MAIN]: Programmet startade


*/

void log_message(const char* module, const char* msg, const char* level, time_t timestamp)
{
    // Öppna loggfilen i tilläggsläge
    FILE* log_file = fopen("server_log.txt", "a");
    if (log_file == NULL) {
        // Om filen inte kan öppnas, skriv till stderr och returnera
        fprintf(stderr, "Kunde inte öppna loggfilen!\n");
        return;
    }

    // Formatera tidsstämpeln
    char time_buffer[26];
    struct tm* tm_info;
    tm_info = localtime(&timestamp);
    strftime(time_buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);

    // Skriv loggmeddelandet till filen
    fprintf(log_file, "[%s] [%s] [%s]: %s\n", time_buffer, level, module, msg);

    // Stäng filen
    fclose(log_file);
}

int main(void)
{
    time_t now;

    /* Hämta aktuell tid */
    now = time(NULL);
    if (now == (time_t)-1) {
        fprintf(stderr, "Kunde inte hämta aktuell tid\n");
        return 1;
    }

    /* Testanrop */
    log_message("MAIN", "Programmet startade", "INFO", now);
    log_message("DATABASE", "Anslutning lyckades", "DEBUG", now);
    log_message("AUTH", "Felaktigt lösenord", "ERROR", now);

    printf("Loggmeddelanden skrivna till server_log.txt\n");
    return 0;
}