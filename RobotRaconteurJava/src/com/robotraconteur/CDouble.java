package com.robotraconteur;

public class CDouble {
	public double real;
	public double imag;
	
	public CDouble(double real, double imag)
	{
		this.real=real;
		this.imag=imag;
	}
	
	@Override
	public boolean equals(Object obj)
	{
		if (obj instanceof CDouble)
		{
			CDouble obj1=(CDouble)obj;
			return this.real==obj1.real && this.imag==obj1.imag; 
		}
		return false;
	}
}
