package com.mridle.rocket2d;

import java.util.HashMap;
import java.util.LinkedList;

import android.graphics.Point;
import android.view.InputDevice;
import android.view.MotionEvent;

public class PointerHelper {
	public static final int POINTER_TYPE_MOUSE	 = 0;
	public static final int POINTER_TYPE_STYLUS  = 1;
	public static final int POINTER_TYPE_TOUCH	 = 2;
	public static final int POINTER_TYPE_UNKNOWN = -1;

	public static final int ACTION_TYPE_PRESSED   = 0;
	public static final int ACTION_TYPE_MOVED	  = 1;
	public static final int ACTION_TYPE_RELEASED  = 2;
	public static final int ACTION_TYPE_CANCELLED = 3;

	public static final HashMap<Integer, Point> pointerPositions = 
			new HashMap<Integer, Point>();
	public static final LinkedList<Point> pointPool = new LinkedList<Point>();

//	private static HashMap<Integer, Point> pointerPos;

	public static void handleMotionEvent(int width, int height, MotionEvent motionEvent) {
		// This is broken and not fixed until api level 14. getSource() only reports
		// tool type of the primary pointer.
		int pointerType;
		switch (motionEvent.getSource()) { 
		case InputDevice.SOURCE_TOUCHSCREEN:
			pointerType = POINTER_TYPE_TOUCH;
			break;
		case 0x4002: // InputDevice.SOURCE_STYLUS added in api level 14
			pointerType = POINTER_TYPE_STYLUS;
			break;
		case InputDevice.SOURCE_MOUSE:
			pointerType = POINTER_TYPE_MOUSE;
			break;
		default:
			pointerType = POINTER_TYPE_UNKNOWN;
			break;
		}

		if (pointerType == POINTER_TYPE_TOUCH || pointerType == POINTER_TYPE_STYLUS || pointerType == POINTER_TYPE_TOUCH) {
			handlePointerEvent(width, height, pointerType, motionEvent);
		}
	}

	private static void handlePointerEvent(int width, int height, int pointerType, MotionEvent motionEvent) {
		int pointerIndex = motionEvent.getActionIndex();
		int pointerId = motionEvent.getPointerId(pointerIndex);

		int actionType;
		int actionMasked = motionEvent.getActionMasked();
		if (actionMasked == MotionEvent.ACTION_POINTER_DOWN || actionMasked == MotionEvent.ACTION_DOWN) {
			actionType = ACTION_TYPE_PRESSED;
		} else if (actionMasked == MotionEvent.ACTION_MOVE) {
			actionType = ACTION_TYPE_MOVED;
		} else if (actionMasked == MotionEvent.ACTION_POINTER_UP || actionMasked == MotionEvent.ACTION_UP) {
			actionType = ACTION_TYPE_RELEASED;
		} else if (actionMasked == MotionEvent.ACTION_CANCEL) {
			actionType = ACTION_TYPE_CANCELLED;
		} else {
			throw new RuntimeException("Unexpected value from action masked");
		}

		if (actionType != ACTION_TYPE_MOVED) {
			int x = (int) motionEvent.getX(pointerIndex);
			int y = (int) motionEvent.getY(pointerIndex);

			if (actionType == ACTION_TYPE_CANCELLED) {
				// Clear all pointer positions
				for (Point p : pointerPositions.values()) {
					pointPool.push(p);
				}
				pointerPositions.clear();
			} else if (actionType == ACTION_TYPE_PRESSED) {
				Point p;
				if (pointPool.isEmpty()) {
					p = new Point();
				} else {
					p = pointPool.pop();
				}
				p.x = x;
				p.y = y;

				pointerPositions.put(pointerId, p);
			} else if (actionType == ACTION_TYPE_RELEASED) {
				Point p = pointerPositions.remove(pointerId);
				pointPool.push(p);
			}
	
			// We're done let's return relevant pointer id and x/y values.
			Rocket2dActivity.onPointerEvent(pointerId, pointerType, actionType, 
					(2*x-width)/(float) width, (height-2*y)/(float) height);
			return;
		}

		// For some mad reason we need to loop through all pointers to see which has changed for move actions.
		int pointerCount = motionEvent.getPointerCount();
		for (pointerIndex = 0; pointerIndex < pointerCount; ++pointerIndex) {
			pointerId = motionEvent.getPointerId(pointerIndex);
			int x = (int) motionEvent.getX(pointerIndex);
			int y = (int) motionEvent.getY(pointerIndex);

			Point oldPosition = pointerPositions.get(pointerId);
			if (x != oldPosition.x || y != oldPosition.y) {
				// Position changed let's update and propagate
				oldPosition.x = x;
				oldPosition.y = y;
				Rocket2dActivity.onPointerEvent(pointerId, pointerType, actionType, 
						(2*x-width)/(float) width, (height-2*y)/(float) height);
			}
		}
	}
}

