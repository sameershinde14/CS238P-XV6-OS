ls > y;cat < y | sort | uniq | wc > y1;
ls > y; (cat < y | sort | uniq | wc > y1;cat y1;) rm y1
ls;pwd
ls & ls -ltr