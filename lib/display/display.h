#ifndef DISPLAY_H
#define DISPLAY_H

void setupDisplay();
void loopDisplay();

bool updateDisplay();
bool updateDisplay(const char *host);

int displaySnapshotBMPStream(uint8_t *buffer, size_t maxLen, size_t index);

#endif