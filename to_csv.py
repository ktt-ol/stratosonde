from __future__ import print_function

import argparse
import sys
import csv
import os
from collections import OrderedDict

sensors = OrderedDict([
    ('time', ['time']),
    ('gps', ['north', 'east', 'knots', 'course', 'date', 'height', 'num_sats']),
    ('millis', ['millis']),
    ('6d', ['ax', 'ay', 'az', 'gx', 'gy', 'gz']),
    ('3d', ['ax', 'ay', 'az']),
    ('lux', ['lux']),
    ('dht', ['temp', 'rel_h']),
    ('uv', ['uv']),
    ('bmp', ['temp', 'pressure']),
])

def row_values(values):
    raw = []
    for s, ms in sensors.items():
        want = len(ms)
        got = len(values.get(s, ''))
        if want != got:
            raw.extend([''] * want)
        else:
            raw.extend(values[s])

    return raw

def headers(sensors):
    for s, ms in sensors.items():
        for m in ms:
            if m == s:
                yield m
            else:
                yield s + '_' + m

def read_gpslog(gps_input):
    gpgga = {}
    for line in gps_input:
        if not line.startswith(b'$GPGGA'):
            continue
        parts = line.split(b',')
        # eg3. $GPGGA,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*hh
        # 1    = UTC of Position
        # 2    = Latitude
        # 3    = N or S
        # 4    = Longitude
        # 5    = E or W
        # 6    = GPS quality indicator (0=invalid; 1=GPS fix; 2=Diff. GPS fix)
        # 7    = Number of satellites in use [not those in view]
        # 8    = Horizontal dilution of position
        # 9    = Antenna altitude above/below mean sea level (geoid)
        # 10   = Meters  (Antenna height unit)
        # 11   = Geoidal separation (Diff. between WGS-84 earth ellipsoid and
        #     mean sea level.  -=geoid is below WGS-84 ellipsoid)
        # 12   = Meters  (Units of geoidal separation)
        # 13   = Age in seconds since last update from diff. reference station
        # 14   = Diff. reference station ID#
        # 15   = Checksum
        time = parts[1]
        num_sats = parts[7]
        meters = parts[9]
        gpgga[time[:6]] = [int(num_sats), meters]
    return gpgga

def process(input, output, gpgga):
    reader = csv.reader(input, delimiter='\t')
    writer = csv.writer(output, delimiter=',')
    writer.writerow(list(headers(sensors)))

    values = {}
    millis = ''
    for row in reader:
        if row[0] == 'time':
            if values:
                writer.writerow(row_values(values))
            values = {}
            values['millis'] = row[1]
        sensor = row[0]
        if sensor not in sensors:
            print("unknown sensor {}".format(sensor), file=sys.stderr)
            continue
        elif sensor == 'gps':
            gprmc = row[2].split(',')
            # 1   220516     Time Stamp
            # 2   A          validity - A-ok, V-invalid
            # 3   5133.82    current Latitude
            # 4   N          North/South
            # 5   00042.24   current Longitude
            # 6   W          East/West
            # 7   173.8      Speed in knots
            # 8   231.8      True course
            # 9   130694     Date Stamp
            # 10  004.2      Variation
            # 11  W          East/West
            # 12  *70        checksum
            if gprmc[2] != 'A':
                continue
            assert gprmc[4] == 'N' and gprmc[6] == 'E'
            north = int(gprmc[3][:2]) + float(gprmc[3][2:]) / 60.0
            east = int(gprmc[5][:3]) + float(gprmc[5][3:]) / 60.0
            kmh = float(gprmc[7]) * 1.852
            ms = kmh / 60 / 60 * 1000
            # ['gps', '0', 'north', 'east', 'ms', 'course', 'date', 'height', 'num_sats']
            course = gprmc[8]
            date = gprmc[9]
            height, num_sats = gpgga.get(gprmc[1][:6], ('', ''))
            row = ['gps', '0', '%.6f' % north, '%.6f' % east, '%.2f' % ms, course, date, height, num_sats]
        values[row[0]] = row[2:]
    else:
        writer.writerow(row_values(values))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('datalog', metavar='data.log')
    parser.add_argument('gpslog', metavar='gps.log')
    parser.add_argument('outputcsv', metavar='output.csv')

    args = parser.parse_args()

    if os.path.exists(args.outputcsv):
        print('ERROR: output file {} exists'.format(args.outputcsv), file=sys.stderr)
        sys.exit(1)

    with open(args.gpslog, 'rb') as gps_input:
        gpgga = read_gpslog(gps_input)
    with open(args.datalog) as input, open(args.outputcsv, 'w') as output:
        process(input, output, gpgga)

