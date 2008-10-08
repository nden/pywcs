import pywcs
import pyfits
import numpy
import sys

hdulist = pyfits.open(sys.argv[-1])

data1 = numpy.array([0,2,4,6])
data2 = numpy.array([1,3,5,7])

header = hdulist[1].header
wcs = pywcs.WCS(header, hdulist)
assert wcs.cpdis1 is not None and wcs.cpdis2 is not None

print data1, data2
focal = wcs.p4_pix2foc(data1, data2)
print "Just P4"
print focal

focal2 = wcs.sip_pix2foc(data1, data2)
print "Just SIP"
print focal2

focal3 = wcs.pix2foc(data1, data2)
print "Both"
print focal3
