package com.example.democlienttcpip;

import java.io.PrintWriter;
import java.util.Timer;
import java.util.TimerTask;

import android.util.Log;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.Toast;

public class CheckBoxListener implements OnCheckedChangeListener {

	private static final String tag = "CheckBoxListener";
	private String text = "";

	private PrintWriter writer = null;

	private Timer timer = null;
	private MyTask task = null;
	private MainActivity mainActivity;

	private EditText edtTimerCycle;

	public CheckBoxListener(MainActivity main) {
		this.mainActivity = main;//把主页面传进来

//		edtTimerCycle = (EditText) main.findViewById(R.id.id_edt_timerCycle);
	}

	/**
	 * 获取定时的时间.
	 * 
	 * @param time
	 */
	public int getTimerCycle(String time) {
		int timeCycle = 0;
		Log.i(tag, "--->>time = " + timeCycle);

		try {
			timeCycle = Integer.valueOf(time);

		} catch (Exception e) {
			timeCycle = -1;
			Log.i(tag, "--->>时间异常...");
		}
		return timeCycle;
	}

	/**
	 * 获取输出流.
	 * 
	 * @param printWriter
	 */
	public void setOutStream(PrintWriter printWriter) {
		this.writer = printWriter;
	}

	public void setSendText(String text) {
		this.text = text;

	}

	@Override
	public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {

		if (isChecked) {
			int timeCycle = getTimerCycle(edtTimerCycle.getText().toString());
			if (timeCycle <= 0) {
				Toast.makeText(mainActivity, "时间异常!", Toast.LENGTH_SHORT)
						.show();
				return;
			}
			if (!mainActivity.isConnected) {
				Toast.makeText(mainActivity, "连接失败!", Toast.LENGTH_SHORT)
						.show();
				return;
			}
			timer = new Timer();

			task = new MyTask();
			setSendText(mainActivity.edtSend.getText().toString());

			timer.scheduleAtFixedRate(task, timeCycle, timeCycle);
			Toast.makeText(mainActivity, "设置成功!", Toast.LENGTH_SHORT).show();
		} else {
			if (task != null) {

				timer.cancel();
				Toast.makeText(mainActivity, "已经取消!", Toast.LENGTH_SHORT)
						.show();

			}
		}
	}

	private class MyTask extends TimerTask {

		@Override
		public void run() {

			if (writer != null) {
				writer.write(text);
				writer.flush();
			}
		}
	}
}
