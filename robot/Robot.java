
import jssc.SerialPort;
import jssc.SerialPortException;

public class Robot{
	
	// Arduino interface
	private static String PORT="/dev/ttyACM0"; // change port here
	
	private SerialPort serialPort;
	
	// linear and angular speed	
	private int vx1=10000;
	private int vy1=10000;
	private int rz1=10000;
	
	
	public Robot() {
		openPort();
	}
	

	// open/close procedures
	public void openPort() {
		try {
			serialPort = new SerialPort(PORT);
			serialPort.openPort();
			serialPort.setParams(9600, 8, 1, 0);
		} catch (SerialPortException e) {e.printStackTrace();}
	}
	
	public void close(){
		try {
			System.out.println("Port closed: " + serialPort.closePort());
		} catch (SerialPortException e) {e.printStackTrace();}
	}
	

	//////////////////////////////////////////////
	// send speed command to Arduino
	public void move(int vx, int vy, int rz) {
		if (vx!=vx1 || vy!=vy1 || rz!=rz1) {	// only send when values change
			sendMsg(vx, vy, rz);
			vx1=vx;
			vy1=vy;
			rz1=rz;
		}
	}
	
	// send command on serial port
	private void sendMsg(int px, int py, int rz){
		
		byte[] msg=new byte[4];
		msg[0]=(byte)255;
		msg[1]=(byte)(px+100);
		msg[2]=(byte)(py+100);
		msg[3]=(byte)(rz+100);

		try {serialPort.writeBytes(msg);
		} catch (SerialPortException e) {e.printStackTrace();}/**/
	}
	
}
