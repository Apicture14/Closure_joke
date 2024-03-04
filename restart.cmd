@echo off
taskkill -im main.exe -f -t
main.exe
echo "success"
pause