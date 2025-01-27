!/bin/bash
# HTML start
echo "<html>"
echo "<body>"

# Calendar title
echo "<h3>Calendar:</h3>"

# Generate a simple calendar for January 2025
echo "    January 2025"
echo "Su Mo Tu We Th Fr Sa"
echo "          1  2  3  4"
echo " 5  6  7  8  9 10 11"
echo "12 13 14 15 16 17 18"
echo "19 20 21 22 23 24 25"
echo "26 27 28 29 30 31"

# Date info
echo "<h3>Date:</h3>"
date

# Days passed since the beginning of the year
days_since_new_year=$(date +%j)  # Days since January 1st
echo "<h4>Days have passed since the beginning of the year:</h4>"
echo "$days_since_new_year"

# HTML end
echo "</body>"
echo "</html>"