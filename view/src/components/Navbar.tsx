// Copyright mzoesch. All rights reserved.

import {Link, NavLink} from 'react-router-dom';
import {Button, Input, Menu, MenuButton, MenuItems} from '@headlessui/react'
import {ChevronDownIcon} from '@heroicons/react/16/solid'
import {useEffect, useState} from 'react';
import {
  ENGINE_URL_STORAGE_KEY,
  deleteStoredEntry, storeValue
  } from "../lib/Storage.tsx";

function EnginePingMenu() {
  const pingStates = {
    Unknown: 'Unknown',
    Pinging: 'Pinging...',
    Successful: 'Success',
    Failed: 'Failed',
  }

  const [status, setStatus] = useState<string>(pingStates.Unknown)
  const statusClass = (() => {
    if (status === pingStates.Successful) return 'bg-teal-950'
    if (status === pingStates.Failed) return 'bg-red-700'
    if (status === pingStates.Pinging) return 'bg-gray-600'
    return 'bg-gray-700'
  })()

  const initialEngineStats = {
    BuildTime: 'N/A',
    BuildDate: 'N/A',
    VcsBranch: 'N/A',
    VcsRevision: 'N/A',

    EngineVersion: 'N/A',

    CompilerVersion: 'N/A',
    CxxStandard: 'N/A',

    TargetPlatform: 'N/A',
    TargetType: 'N/A',
    TargetConfig: 'N/A',
    bEverRender: false,

    Uptime: 0.0,
    Ticks: 0,
    AvgDeltaTime: 0.0,
    AvgTickRate: 0.0,
    MaxDeltaTime: 0.0,
    LowestDeltaTime: 0.0,
    HighestDeltaTime: 0.0,
    HighestLostDeltaTime: 0.0,
    HighestIdleTime: 0.0,
    bTracerPid: false,
    bEverProfile: false,
    bProfiling: false,
  }

  const [engineStats, setEngineStats] = useState<{ [key: string]: string | number | boolean }>(initialEngineStats)
  const [allowPing, setAllowPing] = useState<boolean>(true)
  const [engineUrl, setEngineUrl] = useState<string>(() => {
    return localStorage.getItem(ENGINE_URL_STORAGE_KEY) || ''
  });

  useEffect(() => {
    const onStorage = (e: StorageEvent) => {
      if (e.key === ENGINE_URL_STORAGE_KEY && typeof e.newValue === 'string') {
        setEngineUrl(e.newValue)
      }
    }
    window.addEventListener('storage', onStorage)
    return () => { window.removeEventListener('storage', onStorage) }
  }, []);

  const saveEngineUrl = (value: string) => {
    if (!value || value.length === 0) {
      deleteStoredEntry(ENGINE_URL_STORAGE_KEY)
    } else {
      storeValue(ENGINE_URL_STORAGE_KEY, value)
    }
  }

  const handlePing = async () => {
    console.log('Pinging', engineUrl)

    saveEngineUrl(engineUrl)
    setStatus(pingStates.Pinging)
    setAllowPing(false)

    try {
      const res = await fetch(engineUrl, { method: 'GET' });
      setStatus(res.ok ? pingStates.Successful : pingStates.Failed)

      if (res.ok) {
        const data = await res.json()

        const newStats = { ...initialEngineStats }
        Object.keys(initialEngineStats).forEach((key) => {
          if (key in data && data[key] !== undefined && data[key] !== null) {
            newStats[key] = data[key];
          } else {
            newStats[key] = 'N/A';
          }
        })

        if (newStats['EngineVersion'] === 'N/A') {
          console.error('Invalid response from engine. ["EngineVersion"] is always required: ', data)
          setStatus(pingStates.Failed)
        }

        setEngineStats(newStats)
      }
    } catch (error) {
      console.error('Ping failed:', error)
      setStatus(pingStates.Failed)
    }

    setAllowPing(true)
  }

  return (
    <Menu>
      <MenuButton className='inline-flex items-center gap-2
        rounded-md bg-gray-800 px-3 py-1.5 focus:not-data-focus:outline-none data-focus:outline
        data-focus:outline-white data-hover:bg-gray-700 data-open:bg-gray-700'
      >
        Engine Connection <ChevronDownIcon className='size-4 fill-white/60'/>
      </MenuButton>

      <MenuItems transition anchor='bottom end'
        className='px-2 min-w-96 origin-top-right rounded-sm border border-white/10 bg-slate-800 p-1 text-sm/6 text-white
          flex flex-col gap-2
          transition duration-60 ease-out [--anchor-gap:--spacing(1)] focus:outline-none data-closed:scale-95 data-closed:opacity-0'
      >
        <Input
          className='mt-3 block w-full rounded-lg border-none bg-white/5 px-3 py-1.5 text-sm/6 text-white
            focus:not-data-focus:outline-none data-focus:outline-2 data-focus:-outline-offset-2 data-focus:outline-white/25'
          placeholder='Url'
          value={engineUrl}
          onChange={(e) => setEngineUrl(e.target.value)}
          onBlur={(e) => saveEngineUrl(e.target.value)}
        />
        <div>
          <div className='flex items-center justify-between'>
            <Button className='min-w-36' onClick={handlePing} disabled={!allowPing}>Ping</Button>
            <div className={`text-xl ${statusClass} px-4 border-1 border-white/25 min-w-48 text-center`}>Status: {status}</div>
          </div>
        </div>
        <div style={{ borderTop: '1px solid rgba(255,255,255,0.1)' }} className='mt-2 pt-2 f font-bold text-xl text-white/75'
        >Engin Common Data:</div>
        <div className='flex flex-col gap-1 ml-3 mr-3'>
          {Object.entries(engineStats).map(([key, value]) => (
            <div key={key} className='flex justify-between'>
              <div className='font-bold text-white/75'>{key}:</div>
              <div className='text-white/50'>
                {typeof value === 'number' ? value.toFixed(4) : value.toString()}
              </div>
            </div>
          ))}
        </div>
      </MenuItems>
    </Menu>
  )
}

export default function Navbar() {
  return (
    <nav className='sticky top-0 left-0 flex justify-between items-center px-3 py-2 bg-gray-900' aria-label='Main navigation'>
      <Link style={{ color: 'inherit', }} to={'/'}>
        <div style={{ display: 'flex', flexDirection: 'column', alignItems: 'center', minWidth: '7rem' }}>
          <div style={{ fontWeight: 700, fontSize: '1.1rem' }}>JAFG</div>
          <div style={{ fontWeight: 200, fontSize: '0.8rem' }}>Inspection</div>
        </div>
      </Link>

      <div style={{ display: 'flex', gap: '0.75rem', alignItems: 'center' }}>
        <NavLink to='/' style=
          {({ isActive }) => ({
            color: isActive ? 'antiquewhite' : 'inherit',
            textDecoration: isActive ? 'underline' : 'none',
          })}>Home</NavLink>

        <NavLink to='/console' style=
          {({ isActive }) => ({
            color: isActive ? 'antiquewhite' : 'inherit',
            textDecoration: isActive ? 'underline' : 'none',
          })}>Console</NavLink>

        <EnginePingMenu />
      </div>
    </nav>
  );
}
