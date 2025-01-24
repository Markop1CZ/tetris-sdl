@echo off

py -c "import webbrowser;webbrowser.open(\"http://localhost:8000\")"
py -m http.server