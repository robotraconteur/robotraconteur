package com.robotraconteur;

class RobotRaconteurNativeLoader {

	public static void load()
	{
		String OS=System.getProperty("os.name").toLowerCase();
		String arch=System.getProperty("os.arch").toLowerCase();
		String vendor=System.getProperty("java.vendor").toLowerCase();
		if (OS.indexOf("win")>=0)
		{
			if (arch.equals("x86"))
			{
				System.loadLibrary("RobotRaconteurJavaNative-win32");
				return;
			}
			if (arch.equals("x86_64") || arch.equals("amd64"))
			{
				System.loadLibrary("RobotRaconteurJavaNative-win64");
				return;
			}
			
		}
		
		
		if (vendor.indexOf("android")>=0)
		{
			
			System.loadLibrary("RobotRaconteurJavaNative-android");
			return;
		}
		
		if (OS.indexOf("linux")>=0)
		{
			
			if (arch.equals("x86") || arch.equals("i386"))
			{
				System.loadLibrary("RobotRaconteurJavaNative-linux32");
				return;
			}
			if (arch.equals("x86_64") || arch.equals("amd64"))
			{
				System.loadLibrary("RobotRaconteurJavaNative-linux64");
				return;
			}
			
			if (arch.indexOf("arm")>=0)
			{
				try
				{
					System.loadLibrary("RobotRaconteurJavaNative-linux-arm");
					return;
				}
				catch (java.lang.UnsatisfiedLinkError e) {}
								
				System.loadLibrary("RobotRaconteurJavaNative-linux-armhf");
				return;
				
			}
			
		}
		
		
		
		if (OS.indexOf("mac")>=0)
		{			
				System.loadLibrary("RobotRaconteurJavaNative-darwin");
				return;			
			
		}
		
		throw new RuntimeException("Could not find native library for platform " + OS);
		
	}
	
	
}
