package com.test.BTClient;



import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.UUID;

import com.test.BTClient.DeviceListActivity;
import com.test.BTClient.factivity.ButtonClickListener;

import android.annotation.SuppressLint;
import android.annotation.TargetApi;
import android.app.Activity;
import android.app.AlertDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.view.LayoutInflater;
import android.view.Menu;            //如使用菜单加入此三包
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ScrollView;
import android.widget.TextView;
import android.widget.Toast;

@TargetApi(Build.VERSION_CODES.ECLAIR) @SuppressLint("NewApi") 
public class BTClient extends Activity {
	
	public final static int REQUEST_CONNECT_DEVICE = 1;    //宏定义查询设备句柄
	
	public final static String MY_UUID = "00001101-0000-1000-8000-00805F9B34FB";   //SPP服务UUID号
	
	private InputStream is;    //输入流，用来接收蓝牙数据
	//private TextView text0;    //提示栏解句柄
    private EditText edit0;    //发送数据输入句柄
    private TextView dis;       //接收数据显示句柄
    private ScrollView sv;      //翻页句柄
    public String smsg = "";    //显示用数据缓存
    private String fmsg = "";    //保存用数据缓存
    private String ctr ="";    //保存控制字
    public static boolean isend=false;
    public static String s="";

    public String filename=""; //用来保存存储的文件名
    public BluetoothDevice _device = null;     //蓝牙设备
    public BluetoothSocket _socket = null;      //蓝牙通信socket
    boolean _discoveryFinished = false;    
    boolean bRun = true;
    boolean bThread = false;
	
    public BluetoothAdapter _bluetooth = BluetoothAdapter.getDefaultAdapter();    //获取本地蓝牙适配器，即蓝牙设备
	
    
    private Button start,command;
	
    public void display(String str)
    {
    	Toast.makeText(this, str, Toast.LENGTH_SHORT).show();
    }
    
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);   //设置画面为主画面 main.xml
     //   text0 = (TextView)findViewById(R.id.Text0);  //得到提示栏句柄
        edit0 = (EditText)findViewById(R.id.Edit0);   //得到输入框句柄
        sv = (ScrollView)findViewById(R.id.ScrollView01);  //得到翻页句柄
        dis = (TextView) findViewById(R.id.in);      //得到数据显示句柄

       //如果打开本地蓝牙设备不成功，提示信息，结束程序
        if (_bluetooth == null){
        	Toast.makeText(this, "无法打开手机蓝牙，请确认手机是否有蓝牙功能！", Toast.LENGTH_LONG).show();
            finish();
            return;
        }
        
        // 设置设备可以被搜索  
       new Thread(){
    	   public void run(){
    		   if(_bluetooth.isEnabled()==false){
        		_bluetooth.enable();
    		   }
    	   }   	   
       }.start();   
       
      start=(Button)findViewById(R.id.Button18);
      command=(Button)findViewById(R.id.Button19);
      start.setOnClickListener(new ButtonClickListener());
      command.setOnClickListener(new ButtonClickListener());
    }
    //button的监听器
    class ButtonClickListener implements OnClickListener{
    	public void onClick(View arg0){
    		//todo 切换页面
    		if(arg0.equals(start))
    		{
	    		Intent intent =new Intent();
				//设置要启动的界面
				intent.setClass(BTClient.this,factivity.class);
				//用bundle对象传递数据
			//	Bundle be=new Bundle();
				BTClient.this.startActivity(intent);
				BTClient.this.onPause();
    		}
    		else if(arg0.equals(command))
    		{
    			display("在这啦！别点了");
    		}
    	}
    }

    //发送按键响应
    public void onSendButtonClicked(View v){
    	int i=0;
    	int n=0;
    	try{
    	  final OutputStream os = _socket.getOutputStream();   //蓝牙连接输出流
    		/*byte[] bos = edit0.getText().toString().getBytes();
    		for(i=0;i<bos.length;i++){
    			if(bos[i]==0x0a)n++;
    		}
    		byte[] bos_new = new byte[bos.length+n];
    		n=0;
    		for(i=0;i<bos.length;i++){ //手机中换行为0a,将其改为0d 0a后再发送
    			if(bos[i]==0x0a){
    				bos_new[n]=0x0d;
    				n++;
    				bos_new[n]=0x0a;
    			}else{
    				bos_new[n]=bos[i];
    			}
    			n++;
    		}*/
    		byte[] bos = edit0.getText().toString().getBytes();
    		byte[] bos_new = new byte[bos.length+2];
    		for(i=0;i<bos.length;i++)
    		{
    			bos_new[i]=bos[i];
    			}
    		 //末尾加上0d 0a后再发送
    		bos_new[bos.length]=0x0d;
    		bos_new[bos.length+1]=0x0a;
    		os.write(bos_new);	
    	}catch(IOException e){  		
    	}  	
    }
    
    //接收活动结果，响应startActivityForResult()
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
    	switch(requestCode){
    	case REQUEST_CONNECT_DEVICE:     //连接结果，由DeviceListActivity设置返回
    		// 响应返回结果
            if (resultCode == Activity.RESULT_OK) {   //连接成功，由DeviceListActivity设置返回
                // MAC地址，由DeviceListActivity设置返回
                String address = data.getExtras()
                                     .getString(DeviceListActivity.EXTRA_DEVICE_ADDRESS);
                // 得到蓝牙设备句柄      
                _device = _bluetooth.getRemoteDevice(address);
 
                // 用服务号得到socket
                try{
                	_socket = _device.createRfcommSocketToServiceRecord(UUID.fromString(MY_UUID));
                }catch(IOException e){
                	Toast.makeText(this, "连接失败！", Toast.LENGTH_SHORT).show();
                }
                //连接socket
            	Button btn = (Button) findViewById(R.id.Button13);
                try{
                	_socket.connect();
                	Toast.makeText(this, "连接"+_device.getName()+"成功！", Toast.LENGTH_SHORT).show();
                	btn.setText("断开");
                }catch(IOException e){
                	try{
                		Toast.makeText(this, "连接失败！", Toast.LENGTH_SHORT).show();
                		_socket.close();
                		_socket = null;
                	}catch(IOException ee){
                		Toast.makeText(this, "连接失败！", Toast.LENGTH_SHORT).show();
                	}
                	
                	return;
                }
                
                //打开接收线程
                try{
            		is = _socket.getInputStream();   //得到蓝牙数据输入流
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
            }
    		break;
    	default:break;
    	}
    }
    
    //接收数据线程
    Thread ReadThread=new Thread(){
    	public void run(){
    		isend=false;
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
    					smsg+=s;   //写入接收缓存
    					ctr=s;   //控制字赋值
    					if(is.available()==0) isend=true;s=smsg;break;  //短时间没有数据才跳出进行显示
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
    		if(Trans(ctr,smsg)){}
    		else
    		dis.setText(smsg);   //显示数据 
    		sv.scrollTo(0,dis.getMeasuredHeight()); //跳至数据最后一页
    	}
    };
    
    //关闭程序掉用处理部分
    public void onDestroy(){
    	super.onDestroy();
    	if(_socket!=null)  //关闭连接socket
    	try{
    		_socket.close();
    	}catch(IOException e){}
    //	_bluetooth.disable();  //关闭蓝牙服务
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
        	if(_bluetooth.isEnabled()==false){
        		Toast.makeText(this, "Open BT......", Toast.LENGTH_LONG).show();
        		return true;
        	}
            // Launch the DeviceListActivity to see devices and do scan
            Intent serverIntent = new Intent(this, DeviceListActivity.class);
            startActivityForResult(serverIntent, REQUEST_CONNECT_DEVICE);
            return true;
        case R.id.quit:
            finish();
            return true;
        case R.id.clear:
        	smsg="";
        	dis.setText(smsg);
        	return true;
        case R.id.save:
        	Save();
        	return true;
        }
        return false;
    }
    
    //连接按键响应函数
    public void onConnectButtonClicked(View v){ 
    	if(_bluetooth.isEnabled()==false){  //如果蓝牙服务不可用则提示
    		Toast.makeText(this, " 打开蓝牙中...", Toast.LENGTH_LONG).show();
    		return;
    	}
    	
    	
        //如未连接设备则打开DeviceListActivity进行设备搜索
    	Button btn = (Button) findViewById(R.id.Button13);
    	if(_socket==null){
    		Intent serverIntent = new Intent(this, DeviceListActivity.class); //跳转程序设置
    		startActivityForResult(serverIntent, REQUEST_CONNECT_DEVICE);  //设置返回宏定义
    	}
    	else{
    		 //关闭连接socket
    	    try{
    	    	
    	    	is.close();
    	    	_socket.close();
    	    	_socket = null;
    	    	bRun = false;
    	    	btn.setText("连接");
    	    }catch(IOException e){}   
    	}
    	return;
    }
    
    //保存按键响应函数
    public void onSaveButtonClicked(View v){
    	Save();
    }
    
    //清除按键响应函数
    public void onClearButtonClicked(View v){
    	smsg="";
    	fmsg="";
    	dis.setText(smsg);
    	return;
    }
    
    //退出按键响应函数
    public void onQuitButtonClicked(View v){
    	finish();
    }
    
    //保存功能实现
	public void Save() {
		//显示对话框输入文件名
		LayoutInflater factory = LayoutInflater.from(BTClient.this);  //图层模板生成器句柄
		final View DialogView =  factory.inflate(R.layout.sname, null);  //用sname.xml模板生成视图模板
		new AlertDialog.Builder(BTClient.this)
								.setTitle("文件名")
								.setView(DialogView)   //设置视图模板
								.setPositiveButton("确定",
								new DialogInterface.OnClickListener() //确定按键响应函数
								{
									public void onClick(DialogInterface dialog, int whichButton){
										EditText text1 = (EditText)DialogView.findViewById(R.id.sname);  //得到文件名输入框句柄
										filename = text1.getText().toString();  //得到文件名
										
										try{
											if(Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED)){  //如果SD卡已准备好
												
												filename =filename+".txt";   //在文件名末尾加上.txt										
												File sdCardDir = Environment.getExternalStorageDirectory();  //得到SD卡根目录
												File BuildDir = new File(sdCardDir, "/data");   //打开data目录，如不存在则生成
												if(BuildDir.exists()==false)BuildDir.mkdirs();
												File saveFile =new File(BuildDir, filename);  //新建文件句柄，如已存在仍新建文档
												FileOutputStream stream = new FileOutputStream(saveFile);  //打开文件输入流
												stream.write(fmsg.getBytes());
												stream.close();
												Toast.makeText(BTClient.this, "存储成功！", Toast.LENGTH_SHORT).show();
											}else{
												Toast.makeText(BTClient.this, "没有存储卡！", Toast.LENGTH_LONG).show();
											}
										
										}catch(IOException e){
											return;
										}
										
										
										
									}
								})
								.setNegativeButton("取消",   //取消按键响应函数,直接退出对话框不做任何处理 
								new DialogInterface.OnClickListener() {
									public void onClick(DialogInterface dialog, int which) { 
									}
								}).show();  //显示对话框
	} 

	//对输入的数字处理
	//第一个str是要加上的字符串，第二个str是本身要显示的字符串
	//返回true表示控制字，返回false表示非控制字
	private Boolean Trans(String ctr,String smsg)
	{
		if(ctr.equalsIgnoreCase("1"))
		{
			dis.setText(smsg+"\r\nLED灯已打开");   //显示数据
		}
		else if(ctr.equalsIgnoreCase("2"))
		{
			dis.setText(smsg+"\r\nLED灯已关闭");   //显示数据
		}
		else if(ctr.equalsIgnoreCase("3"))
		{
			dis.setText(smsg+"\r\n声音打开");   //显示数据
		}
		else if(ctr.equalsIgnoreCase("4"))
		{
			dis.setText(smsg+"\r\n声音关闭");   //显示数据
		}
		else if(ctr.equalsIgnoreCase("5"))
		{
			dis.setText(smsg+"\r\n开启监控状态");   //显示数据
		}
		else if(ctr.equalsIgnoreCase("6"))
		{
			dis.setText(smsg+"\r\n关闭监控状态");   //显示数据
		}
		else if(ctr.equalsIgnoreCase("7"))
		{
			dis.setText(smsg+"\r\n获取系统时间");   //显示数据
		}
		else
		{
			ctr="";
			return false;
		//	ctr.setText(ctr);   //显示数据
		}
		ctr="";
		return true;
	}
	
	//通过蓝牙发送数据
	public boolean senddata(byte send[])
	{
		int i=0;
    	int n=0;
    	try{
    		OutputStream os = _socket.getOutputStream();   //蓝牙连接输出流
    		byte[] send_new = new byte[send.length+2];
    		for(i=0;i<send.length;i++)
    		{
    			send_new[i]=send[i];
    		}
    		 //末尾加上0d 0a后再发送
    		send_new[send.length]=0x0d;
    		send_new[send.length+1]=0x0a;
    		os.write(send_new);	
    	}catch(IOException e){  
    		return false;
    	}  	
		return true;
	}
	
	//通过蓝牙接收数据
	/*	public String getdata(){
			 //打开接收线程
            try{
        		is = _socket.getInputStream();   //得到蓝牙数据输入流
        		}catch(IOException e){
        			Toast.makeText(this, "接收数据失败！", Toast.LENGTH_SHORT).show();
        			return "no data";
        		}
        		if(bThread==false){
        			ReadThread.start();
        			bThread=true;
        		}else{
        			bRun = true;
        		}
        		
				return s;
		}*/
}