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
if cluster is not None:
    cur.execute("SELECT address FROM production WHERE cluster="+str(cluster))
    ours = []
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
similar = 0

for o in ours:
    if o in theirs:
        similar += 1

print "Our total: " + str(our_total)
print "Their total: " + str(their_total)
print "Similar: " + str(similar)

