package com.example.democlienttcpip;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.Socket;
import java.nio.charset.Charset;


import android.app.TabActivity;
import android.content.Context;
import android.content.Intent;
import android.media.AudioManager;
import android.media.Ringtone;
import android.media.RingtoneManager;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.os.Vibrator;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.TabHost;
import android.widget.TextView;
import android.widget.Toast;

import android.os.Vibrator;

import android.media.AudioManager;
import android.media.Ringtone;
import android.media.RingtoneManager;

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

import android.content.Context;
import android.os.Bundle;
import android.os.Vibrator;

public class MainActivity<Params> extends TabActivity implements
		OnClickListener {

	private EditText edtIP;
	private EditText edtPort;
	EditText edtSend;
	private EditText edtReceiver;
	private TextView tvReceiver;
	
	private Button btnConn;
	private Button btnSend;

	private Button b1,b2,b3,b4,b5,b6,b7,b8;
	
//	private CheckBox checkBoxTimer;

	private String tag = "MainActivity";

	InputStream in;
	PrintWriter printWriter = null;
	BufferedReader reader;

	Socket mSocket = null;
	public boolean isConnected = false;

	private MyHandler myHandler;

	Thread receiverThread;
	
	Thread play;
	
	CheckBoxListener listener;

	//负责将流中的数据读取，并更新到UI上,这个应该是poll机制，一直在扫描输入流
	private class MyReceiverRunnable implements Runnable {

		public void run() {

			while (true) {

				Log.i(tag, "---->>client receive....");
				if (isConnected) {
					if (mSocket != null && mSocket.isConnected()) {

						String result = readFromInputStream(in);//获取输入流中的数据

						try {
							if (!result.equals("")) {

								Message msg = new Message();//接收到数据，线程交互
								msg.what = 1;
								Bundle data = new Bundle();
								data.putString("msg", result);
								msg.setData(data);
								myHandler.sendMessage(msg);//数据送到handle中
							}

						} catch (Exception e) {
							Log.e(tag, "--->>read failure!" + e.toString());
						}
					}
				}
				try {
					Thread.sleep(100L);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}

			}
		}
	}

	private class MyHandler extends Handler {
		
		VibratorDemoActivity vd=new VibratorDemoActivity();
		@Override
		public void handleMessage(Message msg) {
			super.handleMessage(msg);

			receiverData(msg.what);
			if (msg.what == 1) {
				String result = msg.getData().get("msg").toString();//在主线程handle中接收数据
				tvReceiver.append(result);//数据更新到UI上
				//判断要不要手机发出警报
				if(result.equalsIgnoreCase("r")||result.equalsIgnoreCase("running!\r\n\r\n\r\n")||result.equalsIgnoreCase("unning!\n\n")||result.equalsIgnoreCase("running!\n")||result.equalsIgnoreCase("unning!\n"))
				{
					//new VibratorDemoActivity().send(true);
					vd.send(true);
				}
				else if(result.equalsIgnoreCase("q")||result.equalsIgnoreCase("quit!\r\n\r\n\r\n")||result.equalsIgnoreCase("uit!\n\n")||result.equalsIgnoreCase("quit!\n")||result.equalsIgnoreCase("uit!\n\n"))
				{
				//	new VibratorDemoActivity().onStop();
					vd.send(false);
				}
			}
		}
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {//创建界面
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		init();
	}

		//界面按钮和事件的初始化
	private void init() {

		//获取ip和端口，设置连接/发送的按钮监听
		edtIP = (EditText) this.findViewById(R.id.id_edt_inputIP);
		edtPort = (EditText) this.findViewById(R.id.id_edt_inputport);
		btnSend=(Button) findViewById(R.id.id_btn_send);
		btnSend.setOnClickListener(this);
		btnConn = (Button) findViewById(R.id.id_btn_connClose);
		btnConn.setOnClickListener(this);
		
		b1 = (Button) findViewById(R.id.button1);
		b1.setOnClickListener(this);
		b2 = (Button) findViewById(R.id.button2);
		b2.setOnClickListener(this);
		b3 = (Button) findViewById(R.id.button3);
		b3.setOnClickListener(this);
		b4 = (Button) findViewById(R.id.button4);
		b4.setOnClickListener(this);
		b5 = (Button) findViewById(R.id.button5);
		b5.setOnClickListener(this);
		b6 = (Button) findViewById(R.id.button6);
		b6.setOnClickListener(this);
		b7 = (Button) findViewById(R.id.button7);
		b7.setOnClickListener(this);
		b8 = (Button) findViewById(R.id.button8);
		b8.setOnClickListener(this);
		
	//	edtReceiver=(EditText) this.findViewById(R.id.edtReceiver);
		tvReceiver=(TextView) this.findViewById(R.id.textView2);
		myHandler = new MyHandler();  //新建一个handler处理消息，其中包含了接收输入流的一条数据线路
		play=new Thread(new VibratorDemoActivity());
	}

	//
	/******************************************************************************/
	public String readFromInputStream(InputStream in) {
		int count = 0;
		byte[] inDatas = null;
		try {
			while (count == 0) {
				count = in.available();//获取输入流中可读的长度
			}
			inDatas = new byte[count];
			in.read(inDatas);        //读输入流数据放进数组
			return new String(inDatas, "gb2312");  //返回数据
		} catch (Exception e) {
			e.printStackTrace();
		}
		return null;
	}

	/******************************************************************************/

	@Override
	public void onClick(View v) {

		try
		{
			switch (v.getId()) {

			// 启动2个工作线程:发送、接收。

			case R.id.id_btn_connClose:  //连接按钮

				connectThread();
				break;
			case R.id.id_btn_send:   //发送数据按钮

				sendData("hello!,I'm zq!");
				break;
			case R.id.button1:
				sendData("1");
				break;
				
			case R.id.button2:
				sendData("2");
				break;
			case R.id.button3:
				sendData("3");
				break;
			case R.id.button4:
				sendData("4");
				break;
			case R.id.button5:
				sendData("5");
				break;
			case R.id.button6:
				sendData("6");
				break;
			case R.id.button7:
				sendData("7");
				break;
			case R.id.button8:
				sendData("8");
				break;
			}
		}
		catch (Exception e)
		{
		  showInfo(e.toString());
		}
	}

	/**
	 * 当连接到服务器时,可以触发接收事件.
	 */
	private void receiverData(int flag) {

		if (flag == 2) {
			// mTask = new ReceiverTask();
			receiverThread = new Thread(new MyReceiverRunnable());//开启一个更新UI的线程
			receiverThread.start();

			Log.i(tag, "--->>socket 连接成功!");
			btnConn.setText("断开");

			isConnected = true;
			// mTask.execute(null);
		}

	}

	/**
	 * 发送数据线程.
	 */
	private void sendData(String s) {

		// sendThread.start();
		try {
	//		String context = edtSend.getText().toString();//从编辑框获取要发送的数据
			String context = s;
			if (printWriter == null || context == null) {

				if (printWriter == null) {
					showInfo("连接失败!");
					return;
				}
				if (context == null) {
					showInfo("连接失败!");
					return;
				}
			}

			printWriter.print(context);//数据送输出流
			printWriter.flush();
			Log.i(tag, "--->> client send data!");
		} catch (Exception e) {
			Log.e(tag, "--->> send failure!" + e.toString());

		}
	}

	/**
	 * 启动连接线程.
	 *///启动连接成功后，开启了一个读输入流的线程?
	private void connectThread() {
		if (!isConnected) {//未连接时，
			new Thread(new Runnable() {

				@Override
				public void run() {
					Looper.prepare();//新建一个线程消息传递机制
					Log.i(tag, "---->> connect/close server!");

					connectServer(edtIP.getText().toString(), edtPort.getText()
							.toString());//连接输入框的ip和端口号
				}
			}).start();
		} else {//已经连接时
			try {
				if (mSocket != null) {
					mSocket.close();//关闭连接
					mSocket = null;
					Log.i(tag, "--->>取消server.");
					// receiverThread.interrupt();
				}
			} catch (IOException e) {
				e.printStackTrace();
			}
			btnConn.setText("连接");
			isConnected = false;
		}
	}

	// 连接服务器.(网络调试助手的服务器端编码方式:gb2312)
	private void connectServer(String ip, String port) {
		try {
			Log.e(tag, "--->>start connect  server !" + ip + "," + port);

			mSocket = new Socket(ip, Integer.parseInt(port));//获取一个连接套接字
			Log.e(tag, "--->>end connect  server!");

			OutputStream outputStream = mSocket.getOutputStream();//从套接字获取一个输出流

			printWriter = new PrintWriter(new BufferedWriter(
					new OutputStreamWriter(outputStream,
							Charset.forName("gb2312"))));//转换流的形式
		//	listener.setOutStream(printWriter); //获取输出流  出问题了
			// reader = new BufferedReader(new InputStreamReader(
			// mSocket.getInputStream()));

			in = mSocket.getInputStream();  //从套接字换区一个输入流
			myHandler.sendEmptyMessage(2);  //连接成功，给主线程消息池发送消息，会开启一个扫描输入流的线程

			showInfo("连接成功!");
		} catch (Exception e) {
			isConnected = false;
			showInfo("连接失败！");
			Log.e(tag, "exception:" + e.toString());
		}

	}

	private void showInfo(String msg) {
		Toast.makeText(MainActivity.this, msg, Toast.LENGTH_SHORT).show();

	}

    @Override
    
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }
    
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) 
        {
        	case R.id.action_settings:
        		finish();
        		return true;
        	case R.id.item1:
        		tvReceiver.setText("");
        }
        return true;
    }
    
    class VibratorDemoActivity implements Runnable
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
    	     try
    		 {if (audioManager.getRingerMode() == AudioManager.RINGER_MODE_SILENT) //调成静音会强制退出
    	      {
    			 showInfo("已经调成静音");
    	         return;
    	      } }
    	      catch(Exception e){};
    	      if(b)
    	      {
    		        long [] pattern = {100,400,100,400}; // 停止 开启 停止 开启
    	     	     vibrator.vibrate(pattern,2); //重复两次上面的pattern 如果只想震动一次，index设为-1  在这停了
    	              r.play(); 
    	      }
    	      else
    	      {
    	    	  vibrator.cancel();//取消震动
    	    	  if(r.isPlaying())
    	    		  r.stop();
    	      }
              
    	 }	 
    	 public void onStop()
    	 {
    		  vibrator.cancel();//取消震动
	    	  if(r.isPlaying())
	    		  r.stop();
    	 }
		@Override
		public void run() {
			// TODO Auto-generated method stub
			
		}
    }
	
}

