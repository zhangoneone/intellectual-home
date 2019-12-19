package com.test.BTClient;
import java.io.IOException;
import java.io.InputStream;
import java.util.UUID;

import android.app.Activity;
import android.content.ClipData.Item;
import android.content.Intent;
import android.media.AudioManager;
import android.media.Ringtone;
import android.media.RingtoneManager;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.method.ScrollingMovementMethod;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ScrollView;
import android.widget.TextView;
import android.widget.Toast;

import com.test.BTClient.*;
import android.content.Context;
import android.os.Bundle;
import android.os.Vibrator;

public class factivity extends Activity{
//这是作为主控的界面
	Button bt1,bt2,bt3,bt4,bt5,bt6,bt7,bt8,start,command,config,help;
	
	private TextView dis; 
	private InputStream is;    //输入流，用来接收蓝牙数据
	
    boolean bRun = true;
    boolean bThread = false;
    private String fmsg = "";    //保存用数据缓存
    private String mysmsg= ""; //保存接收到的数据

	BTClient btc=new BTClient();
	
	  public void display(String str)
	    {
	    	Toast.makeText(this, str, Toast.LENGTH_SHORT).show();
	    }
	
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_f);
		bt1=(Button)findViewById(R.id.Button01);
		 bt2=(Button)findViewById(R.id.Button02);
		bt3=(Button)findViewById(R.id.Button07);
		 bt4=(Button)findViewById(R.id.Button08);
		bt5=(Button)findViewById(R.id.Button09);
		bt6=(Button)findViewById(R.id.Button10);
		bt7=(Button)findViewById(R.id.Button11);
		 bt8=(Button)findViewById(R.id.Button12);
		
		start=(Button)findViewById(R.id.Button03);
		command=(Button)findViewById(R.id.Button04);
		config=(Button)findViewById(R.id.Button05);
		 help=(Button)findViewById(R.id.Button06);
		dis=(TextView)findViewById(R.id.Textview02);
//		sv = (ScrollView)findViewById(R.id.scrollView02);  //得到翻页句柄
		bt1.setOnClickListener(new ButtonClickListener());
		bt2.setOnClickListener(new ButtonClickListener());
		bt3.setOnClickListener(new ButtonClickListener());
		bt4.setOnClickListener(new ButtonClickListener());
		bt5.setOnClickListener(new ButtonClickListener());
		bt6.setOnClickListener(new ButtonClickListener());
		bt7.setOnClickListener(new ButtonClickListener());
		bt8.setOnClickListener(new ButtonClickListener());
		command.setOnClickListener(new ButtonClickListener());
		start.setOnClickListener(new ButtonClickListener());
	}
	
	  //菜单处理部分
	   @Override
	    public boolean onCreateOptionsMenu(Menu menu) {//建立菜单
	        MenuInflater inflater = getMenuInflater();
	        inflater.inflate(R.menu.option_menu, menu);
	        return true;
	    }

	    @Override
	    public boolean onOptionsItemSelected(MenuItem item) { //菜单响应函数
	        switch (item.getItemId()) {
	        case R.id.scan:
	        	if(btc._bluetooth.isEnabled()==false){
	        		Toast.makeText(this, "Open BT......", Toast.LENGTH_LONG).show();
	        		return true;
	        	}
	            // Launch the DeviceListActivity to see devices and do scan
	            Intent serverIntent = new Intent(this, DeviceListActivity.class);
	            startActivityForResult(serverIntent, btc.REQUEST_CONNECT_DEVICE);
	            return true;
	        case R.id.quit:
	            finish();
	            return true;
	        case R.id.clear:
	        	mysmsg="";
	        	dis.setText("");
	        	return true;
	        case R.id.save:
	        	btc.Save();
	        	return true;
	        }
	        return false;
	    }
	
	    //接收活动结果，响应startActivityForResult()
	    public void onActivityResult(int requestCode, int resultCode, Intent data) {
	    	switch(requestCode){
	    	case BTClient.REQUEST_CONNECT_DEVICE:     //连接结果，由DeviceListActivity设置返回
	    		// 响应返回结果
	            if (resultCode == Activity.RESULT_OK) {   //连接成功，由DeviceListActivity设置返回
	                // MAC地址，由DeviceListActivity设置返回
	                String address = data.getExtras()
	                                     .getString(DeviceListActivity.EXTRA_DEVICE_ADDRESS);
	                // 得到蓝牙设备句柄      
	                btc._device = btc._bluetooth.getRemoteDevice(address);
	 
	                // 用服务号得到socket
	                try{
	                	btc._socket = btc._device.createRfcommSocketToServiceRecord(UUID.fromString(btc.MY_UUID));
	                }catch(IOException e){
	                	Toast.makeText(this, "连接失败！", Toast.LENGTH_SHORT).show();
	                }
	                //连接socket
	            //    MenuItem it = (MenuItem) findViewById(R.id.scan);
	                try{
	                	btc._socket.connect();
	                	Toast.makeText(this, "连接"+btc._device.getName()+"成功！", Toast.LENGTH_SHORT).show();
	             //   	it.setTitle("断开");
	                }catch(IOException e){
	                	try{
	                		Toast.makeText(this, "连接失败！", Toast.LENGTH_SHORT).show();
	                		btc._socket.close();
	                		btc._socket = null;
	                	}catch(IOException ee){
	                		Toast.makeText(this, "连接失败！", Toast.LENGTH_SHORT).show();
	                	}
	                	
	                	return;
	                }
	            }
	            
                //打开接收线程
                try{
            		is = btc._socket.getInputStream();   //得到蓝牙数据输入流
            		}catch(IOException e){
            			Toast.makeText(this, "接收数据失败！", Toast.LENGTH_SHORT).show();
            			return;
            		}
            		if(bThread==false){
            			ReadThread.start();
            			bThread=true;
            		}else{
            			bRun = true;
            		}
    		break;
    	default:break;
    	}
	    }

//接收数据线程
Thread ReadThread=new Thread(){
	public void run(){
		int num = 0;
		byte[] buffer = new byte[1024];
		byte[] buffer_new = new byte[1024];
		int i = 0;
		int n = 0;
		bRun = true;
		//接收线程
		while(true){
			try{
				while(is.available()==0){
					while(bRun == false){}
				}
				while(true){
					num = is.read(buffer);         //读入数据
					n=0;
					
					String s0 = new String(buffer,0,num);
					fmsg+=s0;    //保存收到数据
					for(i=0;i<num;i++){
						if((buffer[i] == 0x0d)&&(buffer[i+1]==0x0a)){
							buffer_new[n] = 0x0a;
							i++;
						}else{
							buffer_new[n] = buffer[i];
						}
						n++;
					}
					String s = new String(buffer_new,0,n);
					if(s.equalsIgnoreCase("1")||s.equalsIgnoreCase("2") || s.equalsIgnoreCase("3") ||s.equalsIgnoreCase("4") ||s.equalsIgnoreCase("5") || s.equalsIgnoreCase("6") ||s.equalsIgnoreCase("7") ||s.equalsIgnoreCase("8"))
					{
						s="";
					}
					mysmsg+=s;   //写入接收缓存
					
					if(s.equalsIgnoreCase("r")||s.equalsIgnoreCase("running!\n\n")||s.equalsIgnoreCase("unning!\n\n")||s.equalsIgnoreCase("running!\n")||s.equalsIgnoreCase("unning!\n"))
					{
						new VibratorDemoActivity().send(true);
						s="";
					}
					else if(s.equalsIgnoreCase("q")||s.equalsIgnoreCase("quit!\n\n")||s.equalsIgnoreCase("uit!\n\n")||s.equalsIgnoreCase("quit!\n")||s.equalsIgnoreCase("uit!\n\n"))
					{
					  new VibratorDemoActivity().send(false);
					  s="";
					}
					else s="";
					if(is.available()==0)break;  //短时间没有数据才跳出进行显示
				}
				//发送显示消息，进行显示刷新
					handler.sendMessage(handler.obtainMessage());       	  //可能是这里问题，导致回主页面退出  		
	    		}catch(IOException e){
	    		}
		}
	}
};
//消息处理队列
Handler handler= new Handler(){
	public void handleMessage(Message msg){
		super.handleMessage(msg);
		dis.setText(mysmsg);   //显示数据 
	   dis.setMovementMethod(ScrollingMovementMethod.getInstance()) ;
//		sv.scrollTo(0,dis.getMeasuredHeight()); //跳至数据最后一页
	}
};

	class ButtonClickListener implements OnClickListener{
		public void onClick(View arg0){
			//todo
			if(arg0.equals(command))
			{
				Intent intent =new Intent();
				//设置要启动的界面
				intent.setClass(factivity.this,BTClient.class);
				//用bundle对象传递数据
			//	Bundle be=new Bundle();
				factivity.this.startActivity(intent);
				factivity.this.finish();
			}
			else if(arg0.equals(start))
			{
				display("在这啦！别点了");
			}
			else if(arg0.equals(bt1))
			{
				byte[] tmp={49};
				btc.senddata(tmp);//发送1
				display("打开灯");
			}
			else if(arg0.equals(bt2))
			{
				byte[] tmp={50};
				btc.senddata(tmp);//发送2
				display("关闭灯");
			}
			else if(arg0.equals(bt3))
			{
				byte[] tmp={51};
				btc.senddata(tmp);//发送3
				display("打开声音");
			}
			else if(arg0.equals(bt4))
			{
				byte[] tmp={52};
				btc.senddata(tmp);//发送4
				display("关闭声音");
			}
			else if(arg0.equals(bt5))
			{
				byte[] tmp={53};
				btc.senddata(tmp);//发送5
				display("开启监测");
			}
			else if(arg0.equals(bt6))
			{
				byte[] tmp={54};
				btc.senddata(tmp);//发送6
				display("停止监测");
			}
			else if(arg0.equals(bt7))
			{
				byte[] tmp={55};
				btc.senddata(tmp);//发送7
			//	display("获取系统时间");	
			}
			else if(arg0.equals(bt8))
			{
				byte[] tmp={56};
				btc.senddata(tmp);//发送8
			//	display("获取温湿度");
			}
		}
	}
	
	public class VibratorDemoActivity
	{
		protected AudioManager audioManager;
		 protected Vibrator vibrator;
		 protected  Ringtone r;

		 public VibratorDemoActivity()
		 {
			 /** 想设置震动大小可以通过改变pattern来设定，如果开启时间太短，震动效果可能感觉不到* */
			vibrator = (Vibrator)getSystemService(Context.VIBRATOR_SERVICE);
			audioManager = (AudioManager)getSystemService(Context.AUDIO_SERVICE); //此方法是由Context调用的
		    Uri notification = RingtoneManager.getDefaultUri(RingtoneManager.TYPE_RINGTONE); 
            r = RingtoneManager.getRingtone(getApplicationContext(), notification);  
		 }
		 public void send(boolean b)
		 {
		//	 long [] pattern = {100,400,100,400}; // 停止 开启 停止 开启
		//	 vibrator.vibrate(pattern,2); //重复两次上面的pattern 如果只想震动一次，index设为-1
		//	 Thread ctlThread;
		     try
			 {if (audioManager.getRingerMode() == AudioManager.RINGER_MODE_SILENT) //调成静音会强制退出
		      {
		         display("已经调成静音");
		         return;
		      } }
		      catch(Exception e){};
		      if(b)
		      {
			        long [] pattern = {100,400,100,400}; // 停止 开启 停止 开启
		     	     vibrator.vibrate(pattern,2); //重复两次上面的pattern 如果只想震动一次，index设为-1
		              r.play();  
		      }
		      else
		      {
		    	  vibrator.cancel();//取消震动
		    	  if(r.isPlaying())
		    		  r.stop();
		      }
              
		      //开一个线程
		   /*    ctlThread = new Thread() {
		             public void run() {
		            	 try{
		            		 long [] pattern = {100,400,100,400}; // 停止 开启 停止 开启
		            	     vibrator.vibrate(pattern,2); //重复两次上面的pattern 如果只想震动一次，index设为-1
		            	     
		            	     Uri notification = RingtoneManager.getDefaultUri(RingtoneManager.TYPE_RINGTONE); 
		                     Ringtone r = RingtoneManager.getRingtone(getApplicationContext(), notification);  
		                     r.play();  
		            	 }
		            	 catch(Exception e){
		            		 
		            	 }
		             }
		      };*/
		 }	 
		 public void onStop()
		 {
			 
		 }
	}

}
