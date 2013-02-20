#!/usr/bin/env python
# -*- coding: utf-8 -*-

#
# 对比经过和不经过安全宝的服务器响应时间
#

import base64
import os,sys 
import time
import MySQLdb

db_host = ""
db_port = ""
db_user = ""
db_pass = ""
db_name = ""

start_time = '2013-01-30'
end_time = '2013-01-31'

def load_db_conf():
    global db_host
    global db_port
    global db_user
    global db_pass
    global db_name

    dbfile = open('./speed_db.conf')
    for each_line in dbfile.readlines():
        if each_line.startswith('DBHOST'):
            db_host = each_line.split('=')[1].strip()
        elif each_line.startswith('DBPORT'):
            db_port = each_line.split('=')[1].strip()
        elif each_line.startswith('DBUSER'):
            db_user = each_line.split('=')[1].strip()
        elif each_line.startswith('DBPASS'):
            db_pass = each_line.split('=')[1].strip()
        elif each_line.startswith('DBNAME'):
            db_name = each_line.split('=')[1].strip()
    	else:
    		pass
    dbfile.close()


def write_log(str):
    fname = 'conn_%s_%s.result' % (start_time, end_time)
    f = open(fname, 'a')
    f.write(str)
    f.write('\n')
    f.close()


def run_sql(sql):
    global db_host
    global db_port
    global db_user
    global db_pass
    global db_name

    print "RUNSQL: %s" % sql

    try:
        conn = MySQLdb.connect(host=db_host, user=db_user, passwd=db_pass, db=db_name, port=int(db_port))
    except MySQLdb.Error, e:
        print "%s My_sql:connect to mysql error! %d %s" % (time.ctime(), e.args[0], e.args[1])
        time.sleep(1)
        conn = MySQLdb.connect(host=db_host, user=db_user, passwd=db_pass, db=db_name)

    cur = conn.cursor(MySQLdb.cursors.DictCursor)
    try:
        cur.execute(sql)
    except MySQLdb.Error, e:
        print "%s My_sql:operate mysql error! %d %s" % (time.ctime(), e.args[0], e.args[1])

    records = cur.fetchall()
    conn.commit()
    cur.close()
    conn.close()
    return records


def get_response_time(site, suffix, start, end):
    sql = 'select sum(response_time) as total_time from speed_monitor_data where role_name="%s_%s" and monitor_time > "%s" and monitor_time < "%s"' % (site, suffix, start, end)
    result = run_sql(sql)
    if len(result):
        resp_time = result[0]['total_time']
    
    sql = 'select count(*) as count from speed_monitor_data where role_name="%s_%s" and monitor_time > "%s" and monitor_time < "%s"' % (site, suffix, start, end)
    result = run_sql(sql)
    if len(result):
        count = result[0]['count']
    
    return (resp_time, count, resp_time/count)


def get_domain_name(str):
    return str.split('_')[0].strip()


if __name__ == '__main__':
    load_db_conf()

    sql = 'select distinct(role_name) from speed_monitor_data where role_name like "%%_aqb" and monitor_time > "%s" and monitor_time < "%s"' % (start_time, end_time)
    sites = run_sql(sql)
    if len(sites):
        for site in sites:
            site_name = get_domain_name(site['role_name'])
            (r1, c1, d1) = get_response_time(site_name, 'aqb', start_time, end_time)
            (r2, c2, d2) = get_response_time(site_name, 'ip', start_time, end_time)
            write_log('%s\t%s\t%s\t%s\t%s\t%s\t%s' % (site_name, r1, c1, d1, r2, c2, d2))


