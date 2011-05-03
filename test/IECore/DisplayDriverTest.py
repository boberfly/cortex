##########################################################################
#
#  Copyright (c) 2008-2011, Image Engine Design Inc. All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are
#  met:
#
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#
#     * Neither the name of Image Engine Design nor the names of any
#       other contributors to this software may be used to endorse or
#       promote products derived from this software without specific prior
#       written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
#  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
#  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
#  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
#  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
#  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
#  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
##########################################################################

import unittest
import os
import gc
import glob
import sys
import time
from IECore import *

class TestImageDisplayDriver(unittest.TestCase):

	def testConstruction( self ):
		idd = ImageDisplayDriver( Box2i( V2i(0,0), V2i(100,100) ), Box2i( V2i(10,10), V2i(40,40) ), [ 'r','g','b' ], CompoundData() )
		self.assertEqual( idd.scanLineOrderOnly(), False )
		self.assertEqual( idd.displayWindow(), Box2i( V2i(0,0), V2i(100,100) ) )
		self.assertEqual( idd.dataWindow(), Box2i( V2i(10,10), V2i(40,40) ) )
		self.assertEqual( idd.channelNames(), [ 'r', 'g', 'b' ] )

	def __prepareBuf( self, buf, width, offset, red, green, blue ):
		for i in xrange( 0, width ):
			buf[3*i] = blue[i+offset]
			buf[3*i+1] = green[i+offset]
			buf[3*i+2] = red[i+offset]

	def testComplete( self ):

		img = Reader.create( "test/IECore/data/tiff/bluegreen_noise.400x300.tif" )()
		idd = ImageDisplayDriver( img.displayWindow, img.dataWindow, list( img.channelNames() ), CompoundData() )
		self.assertEqual( img.keys(), [ 'B', 'G', 'R' ] )
		red = img['R'].data
		green = img['G'].data
		blue = img['B'].data
		width = img.dataWindow.max.x - img.dataWindow.min.x + 1
		buf = FloatVectorData( width * 3 )
		for i in xrange( 0, img.dataWindow.max.y - img.dataWindow.min.y + 1 ):
			self.__prepareBuf( buf, width, i*width, red, green, blue )
			idd.imageData( Box2i( V2i( img.dataWindow.min.x, i + img.dataWindow.min.y ), V2i( img.dataWindow.max.x, i + img.dataWindow.min.y) ), buf )
		idd.imageClose()
		self.assertEqual( idd.image(), img )

	def testFactory( self ):

		idd = DisplayDriver.create( "ImageDisplayDriver", Box2i( V2i(0,0), V2i(100,100) ), Box2i( V2i(10,10), V2i(40,40) ), [ 'r', 'g', 'b' ], CompoundData() )
		self.failUnless( isinstance( idd, ImageDisplayDriver ) )
		self.assertEqual( idd.scanLineOrderOnly(), False )
		self.assertEqual( idd.displayWindow(), Box2i( V2i(0,0), V2i(100,100) ) )
		self.assertEqual( idd.dataWindow(), Box2i( V2i(10,10), V2i(40,40) ) )
		self.assertEqual( idd.channelNames(), [ 'r', 'g', 'b' ] )

		# test if all symbols are gone after the tests.
		creator = None
		idd = None
		gc.collect()
		RefCounted.collectGarbage()
		self.assertEqual( RefCounted.numWrappedInstances(), 0 )
		
	def testImagePool( self ) :
	
		img = Reader.create( "test/IECore/data/tiff/bluegreen_noise.400x300.tif" )()
		
		idd = DisplayDriver.create(
			"ImageDisplayDriver",
			img.displayWindow,
			img.dataWindow,
			list( img.channelNames() ),
			{
				"handle" : StringData( "myHandle" )
			}
		)
		
		red = img['R'].data
		green = img['G'].data
		blue = img['B'].data
		width = img.dataWindow.max.x - img.dataWindow.min.x + 1
		buf = FloatVectorData( width * 3 )
		for i in xrange( 0, img.dataWindow.max.y - img.dataWindow.min.y + 1 ):
			self.__prepareBuf( buf, width, i*width, red, green, blue )
			idd.imageData( Box2i( V2i( img.dataWindow.min.x, i + img.dataWindow.min.y ), V2i( img.dataWindow.max.x, i + img.dataWindow.min.y) ), buf )

		idd.imageClose()

		self.assertEqual( ImageDisplayDriver.storedImage( "myHandle" ), idd.image() )
		self.assertEqual( ImageDisplayDriver.removeStoredImage( "myHandle" ), idd.image() )
		self.assertEqual( ImageDisplayDriver.storedImage( "myHandle" ), None )
		
		
class TestClientServerDisplayDriver(unittest.TestCase):

	def setUp( self ):

		gc.collect()
		RefCounted.collectGarbage()
		# make sure we don't have symbols from previous tests
		self.assertEqual( RefCounted.numWrappedInstances(), 0 )

		# this is necessary so python will allow threads created by the display driver server
		# to enter into python when those threads execute procedurals.
		initThreads()

		self.server = DisplayDriverServer( 1559 )
		time.sleep(2)

	def __prepareBuf( self, buf, width, offset, red, green, blue ):
		for i in xrange( 0, width ):
			buf[3*i] = blue[i+offset]
			buf[3*i+1] = green[i+offset]
			buf[3*i+2] = red[i+offset]

	def testTransfer( self ):

		img = Reader.create( "test/IECore/data/tiff/bluegreen_noise.400x300.tif" )()
		self.assertEqual( img.keys(), [ 'B', 'G', 'R' ] )
		red = img['R'].data
		green = img['G'].data
		blue = img['B'].data
		width = img.dataWindow.max.x - img.dataWindow.min.x + 1

		params = CompoundData()
		params['displayHost'] = StringData('localhost')
		params['displayPort'] = StringData( '1559' )
		params["remoteDisplayType"] = StringData( "ImageDisplayDriver" )
		params["handle"] = StringData( "myHandle" )
		idd = ClientDisplayDriver( img.displayWindow, img.dataWindow, list( img.channelNames() ), params )

		buf = FloatVectorData( width * 3 )
		for i in xrange( 0, img.dataWindow.max.y - img.dataWindow.min.y + 1 ):
			self.__prepareBuf( buf, width, i*width, red, green, blue )
			idd.imageData( Box2i( V2i( img.dataWindow.min.x, i + img.dataWindow.min.y ), V2i( img.dataWindow.max.x, i + img.dataWindow.min.y) ), buf )
		idd.imageClose()
		
		newImg = ImageDisplayDriver.removeStoredImage( "myHandle" )
		params["clientPID"] = IntData( os.getpid() )
		self.assertEqual( newImg.blindData(), params )
		# remove blindData for comparison
		newImg.blindData().clear()
		img.blindData().clear()
		self.assertEqual( newImg, img )

	def tearDown( self ):
		
		self.server = None
		
		# test if all symbols are gone after the tests.
		gc.collect()
		RefCounted.collectGarbage()
		self.assertEqual( RefCounted.numWrappedInstances(), 0 )

if __name__ == "__main__":
	unittest.main()

