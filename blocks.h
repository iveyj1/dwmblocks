//Modify this file to change what commands output to your statusbar, and recompile using the make command.
static const Block blocks[] = {
	/*Icon*/	/*Command*/		/*Update Interval*/	/*Update Signal*/
	{"  ", "date '+%a %b %d %I:%M%p'",                                  1,      0},
	{"󰖀 ", "wpctl get-volume @DEFAULT_AUDIO_SINK@ | awk '{printf \"%d%%\", $2 * 100; if ($3 == \"[MUTED]\") printf \" muted\"; print \"\"}'", 1,0},
	{"", "dwmblocks-wifi", 1, 0},
    {"CPU ", "top -bn1 | awk '/Cpu/ {print int(100 - $8) \"%\"}'", 5, 0},
	{"", "free --mega | awk '/^Mem/ { print $3\"/\"$2 }' | sed s/i//g",     5,     0},
    // {"", "awk '{printf \"%s%% \", $1}' /sys/class/power_supply/BAT0/capacity; awk '{printf \"%s \", $1 $2}' /sys/class/power_supply/BAT0/status", 10, 0},
    {"", "dwmblocks-charge-rate", 10, 0},
};

# 
// sets delimiter between status commands. NULL character ('\0') means no delimiter.
static char delim[] = " | ";
static unsigned int delimLen = 5;
