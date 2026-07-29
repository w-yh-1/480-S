#pragma once
void outPutFunc(void *data, size_t len, uint8_t type, bool isEnd);
void start_rtp_send(const char *remoteRtpIp, int remoteRtpPort, int localRtpPort, bool isTcp=false, uint32_t ssrc=0);
void stop_rtp_send();
