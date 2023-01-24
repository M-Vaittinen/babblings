#!/bin/bash

#  -d, --duration=N                Duration of the presentation (in minutes)
#  -e, --end-time=HH:MM            End time of the presentation

DUR="30"
#ENDTIM='00:25'
MY_SCREEN=eDP-1
#MY_SCREEN=DP-1-3
#AUDIENCE_SCREEN=DP-1-2
AUDIENCE_SCREEN=DP-1
START=13:30
END=14:00

#Display monitors
#pdfpc -M
#present
#pdfpc --presenter-screen=eDP-1 --notes=top pmic_babbling.pdf
#Present using defined presentation monitor
#pdfpc --presenter-screen=eDP-1 --presentation-screen=M --notes=top pmic_babbling.pdf
#pdfpc --presenter-screen=eDP-1 --presentation-screen=DP-1-2 --notes=top -d="$DUR" -e="$ENDTIM" pmic_babbling.pdf
#pdfpc --presenter-screen="$MY_SCREEN" --presentation-screen="$AUDIENCE_SCREEN" --notes=top -d "$DUR" -e "$ENDTIM" pmic_babbling.pdf
#pdfpc --presenter-screen="$MY_SCREEN" --presentation-screen="$AUDIENCE_SCREEN" --notes=top -d "$DUR" pmic_babbling.pdf
pdfpc --presenter-screen="$MY_SCREEN" --presentation-screen="$AUDIENCE_SCREEN" --notes=top --start-time=$START --end-time=$END pmic_babbling.pdf
#pdfpc --presenter-screen="$MY_SCREEN" --presentation-screen="$AUDIENCE_SCREEN" --notes=top -d="$DUR" pmic_babbling.pdf
#pdfpc --presentation-screen=DP-1-2 --notes=top pmic_babbling.pdf
