////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Computer Graphics Group RWTH Aachen University         //
// All rights reserved.                                                       //
////////////////////////////////////////////////////////////////////////////////

/*
 *  FileHelpersiOS.c
 *  iOS-Test
 *
 *
 */

#include <ACGL/Utils/FileHelpers.h>
#import <UIKit/UIKit.h>

std::string getDeviceDependentPathFor( const std::string &resource )
{
	NSString *res   = [NSString stringWithFormat:@"%s", resource.c_str() ];
	NSString *path = [[NSBundle mainBundle] pathForResource: res ofType: nil ];
	
    if (path) {
        return std::string( [path cStringUsingEncoding:1]  );
    }
    return ""; // file not found
}
