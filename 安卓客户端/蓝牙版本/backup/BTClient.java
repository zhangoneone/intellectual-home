package com.test.BTClient;



import java.io.IOException;
import java.io.OutputStream;
import java.util.UUID;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

public class BTClient extends Activity {
	
	private TextView text0;
    private EditText edit0;
    
  //按钮动作，监听服务端口，打开UUID服务

    BluetoothDevice _device = null;
    BluetoothSocket _socket = null;
    boolean _discoveryFinished = false;
	
    private BluetoothAdapter _bluetooth = BluetoothAdapter.getDefaultAdapter();
	
	
	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        
        text0 = (TextView)findViewById(R.id.Text0);
        edit0 = (EditText)findViewById(R.id.Edit0);

        
      //打开本地设备
        if (_bluetooth == null){
        	Toast.makeText(this, "Bluetooth is not available", Toast.LENGTH_LONG).show();
          //  finish();
            return;
        }
        
        // 设置设备可以被搜索  
        _bluetooth.enable();
        if(_bluetooth.isEnabled()==false){
        	Toast.makeText(this, "Bluetooth can`t be discorvered", Toast.LENGTH_LONG).show();
        	//finish();
        	return;
        }
        
        
        text0.setText("打开本地蓝牙服务成功!");
    }
    

    //定义广播接收器
    private BroadcastReceiver _foundReceiver = new BroadcastReceiver() {
		public void onReceive(Context context, Intent intent) {
			/* 从intent中取得搜索结果数据 */
			//BluetoothDevice _device0 = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
			//if (_device0.getName().equals("linvor") )
				_device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);;
			text0.setText("找寻到设备:"+_device.getName());
		}
	};
	private BroadcastReceiver _discoveryReceiver = new BroadcastReceiver() {

		@Override
		public void onReceive(Context context, Intent intent) 
		{
			/* 卸载注册的接收器 */
			unregisterReceiver(_foundReceiver);
			unregisterReceiver(this);
			_discoveryFinished = true;
		}
	};
    
    //连接搜寻服务
	public void onSearchButtonClicked(View v){
		
		IntentFilter discoveryFilter = new IntentFilter(BluetoothAdapter.ACTION_DISCOVERY_FINISHED);
		registerReceiver(_discoveryReceiver, discoveryFilter);
		IntentFilter foundFilter = new IntentFilter(BluetoothDevice.ACTION_FOUND);
		registerReceiver(_foundReceiver, foundFilter);
		new Thread(){
    		public void run() 
    		{
    			//开始搜索 
    			_bluetooth.startDiscovery();
    			for (;;) 
    			{
    				if (_discoveryFinished) 
    				{
    					break;
    				}
    				try 
    				{
    					Thread.sleep(100);
    				} 
    				catch (InterruptedException e){}
    			}
    		}
    	}.start();
    	text0.setText("正在搜寻设备");

	}
	
	//连接设备
    public void onConenectButtonClicked(View v){
    	/*_device = _bluetooth.getRemoteDevice("00:10:20:26:00:04");
    	
    	if(_device==null){
    		Toast.makeText(this, "Bluetooth is not found", Toast.LENGTH_LONG).show();
    		return;
    	}
    	
    	
    	
    
    	//链接设备
    	try{
    		_bluetooth.cancelDiscovery();
    		
    		_socket = _device.createRfcommSocketToServiceRecord(UUID.fromString("00001101-0000-1000-8000-00805F9B34FB"));
    		text0.setText("打开服务成功");
    	}catch(IOException e){
    		text0.setText("打开服务不成功");
    		return;
    	}
    	
    	if(_socket==null){
    		text0.setText("打开连接失败");
    		return;
    	}*/
    	try{
    		_socket.connect();
    		text0.setText("成功建立连接，可以发送数据！"+_device.getName());
    	}catch(IOException e){
    		//text0.setText(e.toString());
    		text0.setText("连接失败");
    	}
    	
    	
    }
    
    public void onSendButtonClicked(View v){
    	try{
    		OutputStream os = _socket.getOutputStream();
    		os.write(edit0.getText().toString().getBytes());
    		os.close();
    		text0.setText("发送数据成功！");
    		
    	}catch(IOException e){
    		
    	}
    	
    }
    
    public void onDestroy(){
    	super.onDestroy();
    	if(_socket!=null)
    	try{
    		_socket.close();
    	}catch(IOException e){}
    }
}