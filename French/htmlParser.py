#!/usr/bin/python2.7
from bs4 import BeautifulSoup as Soup
import calendar
class HTMLCalendarAdder:
    def __init__(self, html):
        self.soup = Soup(html, "html.parser")
        self.classname = "assignment"
        self.tagname = "li"
        self.datemap = {v: k for k, v in enumerate(calendar.month_abbr)}

    def __format(self, date, title, uid):
        print "BEGIN:VEVENT"
        print "DTSTART:2016"+str(date)+"T090000"
        print "DTSTAMP:20161002T170411"
        print "UID:" + str(uid)
        print "SUMMARY:CANVAS "+title
        print "END:VEVENT"

    def generate(self):
        assignments = self.soup.find_all(self.tagname, {"class":self.classname})
        for assignment in assignments:
            uid=assignment.find("div").get("id").replace("_", "")
            info=assignment.find("div", {"class": "ig-info"})
            duedate=info.find("span").contents[0]
            duedate=str(duedate).strip().split(" ")
            month=str(self.datemap[duedate[0]])
            day=str(duedate[1])
            if len(day) == 1:
                day = "0" + day
            title=info.find("a").contents[0].strip()
            title = ''.join(e for e in title if e.isalnum())
            self.__format(month + day, title, uid)

print "BEGIN:VCALENDAR"
print "PRODID:-//Office of Information and Technology, University of California in Irvine//NONSGML Calendar Course Download//EN"
print "VERSION:2.0"
print "METHOD:PUBLISH"
print "CALSCALE:GREGORIAN"
print "X-WR-TIMEZONE:America/Los_Angeles"
print "BEGIN:VTIMEZONE"
print "TZID:America/Los_Angeles"
print "X-LIC-LOCATION:America/Los_Angeles"
print "BEGIN:DAYLIGHT"
print "TZOFFSETFROM:-0800"
print "TZOFFSETTO:-0700"
print "TZNAME:PDT"
print "DTSTART:19700308T020000"
print "RRULE:FREQ=YEARLY;BYMONTH=3;BYDAY=2SU"
print "END:DAYLIGHT"
print "BEGIN:STANDARD"
print "TZOFFSETFROM:-0700"
print "TZOFFSETTO:-0800"
print "TZNAME:PST"
print "DTSTART:19701101T020000"
print "RRULE:FREQ=YEARLY;BYMONTH=11;BYDAY=1SU"
print "END:STANDARD"
print "END:VTIMEZONE"
parser=HTMLCalendarAdder(open("assignments.html").read())
parser.generate()
print "END:VCALENDAR"
