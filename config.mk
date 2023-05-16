#----------------------------- User configuration -----------------------------

LOGGER_IP := 127.0.0.1
# Crash the game if the logging server wasn't found
ABORT_IF_NO_SERVER := 1
# Enable logging and drawing the debug menu
SMO_DEBUG := 1

# Settings for deploying over FTP. Used by the deploy-ftp.py script.
FTP_IP := 192.168.178.104
FTP_USERNAME := cookieso
FTP_PASSWORD := 0

# Settings for deploying to Ryu. Used by the deploy-ryu.sh script.
RYU_PATH := /home/cookieso/.var/app/org.ryujinx.Ryujinx/config/Ryujinx
