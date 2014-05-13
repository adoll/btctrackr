import sys
import MySQLdb
import urllib
import json

db = MySQLdb.connect(host="localhost",
                     user="root",
                     passwd="privacy",
                     db="production")

cur = db.cursor()

cur.execute("SELECT cluster FROM production WHERE address LIKE \"" + sys.argv[1] +"\"")
cluster = int(cur.fetchone()[0])

cur.execute("SELECT address FROM production WHERE cluster="+str(cluster))
ours = []
if cur.fetchall() is not None:
    for row in cur.fetchall():
        ours += row

url = "http://bitiodine.net/api/cluster/" + sys.argv[1]
iodine = urllib.urlopen(url)
result = iodine.read()
decoded = json.loads(result)

theirs = []

if decoded['data'] is not None:
    for addr in decoded['data']:
        theirs.append(addr['address']['address'])

our_total = len(ours)
their_total = len(theirs)
ours_in_theirs = 0
theirs_in_ours = 0

for o in ours:
    if o in theirs:
        ours_in_theirs += 1

for t in theirs:
    if t in ours:
        theirs_in_ours += 1

print "Our total: " + str(our_total)
print "Their total: " + str(their_total)
print "Ours in Theirs: " + str(ours_in_theirs)
print "Theirs in Ours: " + str(theirs_in_ours)

