#!/usr/bin/python2.7
from json import JSONDecoder as dec
import calendar
import unidecode
class JSONCalenderAdder:
    def __init__(self, s):
        self.dec = dec()
        self.data = self.dec.decode(s)
        self.datemap = {v: k for k, v in enumerate(calendar.month_abbr)}

    def info(self):
        print self.datemap

    def __format(self, date, title, uid):
        print "BEGIN:VEVENT"
        print "DTSTART:2016"+str(date)+"T100000"
        print "DTSTAMP:20161002T170411"
        print "UID:" + str(uid)
        print "SUMMARY:"+title
        print "END:VEVENT"

    def generate(self):
        assignments = self.data["Results"]["dayInfoArray"]
        for day in assignments:
            reminderdate = day["date"]
            reminderdate = reminderdate.split(" ")
            month = str(self.datemap[str(reminderdate[1])[:3]])
            daynum = str(reminderdate[2])
            if len(daynum) == 1:
                daynum = "0" + daynum
            
            # get each assignment
            for entry in day["assignments"]:
                info = entry["activityInfo"]
                exnum,title = (info["exNum"][1:],info["title"][1:])
                exnum = unidecode.unidecode(exnum[exnum.index(">")+1:exnum.index("<")])
                exnum = ''.join(e for e in exnum if e.isalnum())
                title = unidecode.unidecode(title[title.index(">")+1:title.index("<")])
                title = ''.join(e for e in title if e.isalnum())
                activityid=info["activity_id"]
                self.__format(str(month)+daynum, str(exnum) + " " + title, activityid)

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
for f in ["week1.json", "week2.json", "week3.json"]:
    cal=JSONCalenderAdder(open(f).read())
    cal.generate()
print "END:VCALENDAR"
