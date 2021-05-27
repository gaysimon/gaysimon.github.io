import java.io.BufferedInputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;



/**
 * communication between the robot and the PC
 * @author simon
 *
 */
public class Interface {


	private File file = new File("/dev/ttyUSB0" ); 
	
	private FileInputStream fis;
	private BufferedInputStream bis;
	private DataInputStream dis;
	
	private FileOutputStream fos; 
	private DataOutputStream dos;

	//////////////////////////////////////////////////////////////////////
	// sensors
	private int result;

	//--------------------------------------------------------------------
	
	// parameter: serial port (for example, /dev/ttyUSB0 )
	public Interface(String serial){

		file=serial;	

		try{
			fis = new FileInputStream(file);
			bis = new BufferedInputStream(fis);
			dis = new DataInputStream(bis);
		
			fos= new FileOutputStream(file); 
			dos=new DataOutputStream(fos);
		}
		catch(Exception ie){ }
	}
	
	
	// robot command
	public void move(int msg){
		
		// send action
		try{
			// packet head
			dos.write((byte)0x55);
			dos.write((byte)0xff);
			dos.write((byte)0x00);
			
			dos.write((byte) 0xf7);
			dos.write((byte) 0x08);
	
			// msg
			dos.write((byte) msg);
			dos.write((byte)~msg);
			
			// sum
			dos.write((byte) (0xf7+msg));
			dos.write((byte)~(0xf7+msg));
		}
		catch (IOException e) {e.printStackTrace();}
		

		byte data;
		
		// get the response
		try{
			while (this.dis.available() <18 ){
				try{Thread.currentThread().sleep(10);}
				catch(Exception ie){ }
			}
			int i=0;
			while (this.dis.available() != 0){
				data= dis.readByte();
				if (i==14){
					result=data;
				}
				i++;
			}
		}
		catch (IOException e) {{
			e.printStackTrace();}
		}
	}
	
	// return the last received message
	public int getResult(){
		return result;
	}
}
