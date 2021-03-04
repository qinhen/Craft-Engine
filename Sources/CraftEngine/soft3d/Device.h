#pragma once
#include "./Common.h"

#include "./Memory.h"


namespace CraftEngine
{
	namespace soft3d
	{

	

		namespace detail
		{
			struct DeviceData
			{
				core::ThreadPool mThreadPool;
				uint32_t         mCurThread;
				bool             mNoBlock;
				std::atomic_int  mThreadWorking;
			};
		}


		class Device
		{
		private:
			detail::DeviceData* m_deviceData;
		public:
			Device(void* handle) :m_deviceData((detail::DeviceData*)handle) { }
			Device() :m_deviceData(nullptr) { }

		public:
			void waitDevice()
			{
				m_deviceData->mThreadPool.wait();
			}
			void stopDevice()
			{
				m_deviceData->mThreadPool.stop();
			}
			bool allFinished() const
			{
				return m_deviceData->mThreadPool.finished();
			}
			void resetDevice(int count)
			{
				waitDevice();
				m_deviceData->mThreadPool.init(count);
			}
			void* handle() const { return m_deviceData; }
			bool  valid() const { return handle() != nullptr; }
		};




		Device createDevice()
		{
			auto device_data = new detail::DeviceData;
			Device device(device_data);
			device_data->mThreadPool.init(math::max(int(std::thread::hardware_concurrency()), 1));
			device_data->mCurThread = 0;
			device_data->mNoBlock = false;
			return device;
		}

		void destroyDevice(Device& device)
		{
			auto device_data = (detail::DeviceData*)device.handle();
			device_data->mThreadPool.wait();
			device_data->mThreadPool.init(0);
			delete device_data;
		}




	}
}

