import {Link} from 'react-router-dom';
import {Button, Input, Menu, MenuButton, MenuItems} from '@headlessui/react'
import {ChevronDownIcon} from '@heroicons/react/16/solid'
import {useEffect, useState} from 'react';

function GetEnginePingMenu() {
  const ENGINE_URL_STORAGE_KEY = 'engine_url'

  const pingStates = {
    Unknown: 'Unknown',
    Pinging: 'Pinging...',
    Successful: 'Success',
    Failed: 'Failed',
  }

  const engineStats = {
    Version: 'N/A',
    Uptime: 'N/A',
    Requests: 'N/A',
    Errors: 'N/A',
  }

  const [status, setStatus] = useState<string>(pingStates.Unknown)
  const [allowPing, setAllowPing] = useState<boolean>(true)
  const [engineUrl, setEngineUrl] = useState<string>(() => {
    return localStorage.getItem(ENGINE_URL_STORAGE_KEY) ?? 'http://localhost:8080'
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
    localStorage.setItem(ENGINE_URL_STORAGE_KEY, value)
  }

  const handlePing = async () => {
    console.log('Pinging', engineUrl)

    saveEngineUrl(engineUrl)
    setStatus(pingStates.Pinging)
    setAllowPing(false)

    try {
      await new Promise((resolve) => setTimeout(resolve, 500));

      const res = await fetch(engineUrl, { method: 'GET' });
      setStatus(res.ok ? pingStates.Successful : pingStates.Failed)

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
            <div className='text-xl bg-teal-950 px-4 border-1 border-white/25 min-w-48 text-center'>Status: {status}</div>
          </div>
        </div>
        <div style={{ borderTop: '1px solid rgba(255,255,255,0.1)' }} className='mt-2 pt-2 f font-bold text-xl text-white/75'
        >Engin Common Data:</div>
        <div className='flex flex-col gap-1 ml-3 mr-3'>
          {Object.entries(engineStats).map(([key, value]) => (
            <div key={key} className='flex justify-between'>
              <div className='font-bold text-white/75'>{key}:</div>
              <div>{value}</div>
            </div>
          ))}
        </div>
      </MenuItems>
    </Menu>
  )
}

export default function Navbar() {
  return (
    <nav style={{
      display: 'flex',
      justifyContent: 'space-between',
      alignItems: 'center',
      padding: '0.2rem 0.75rem',
      backgroundColor: '#2f343f',
    }} aria-label='Main navigation'>
      <Link style={{ color: 'inherit', }} to={'/'}>
        <div style={{ display: 'flex', flexDirection: 'column', alignItems: 'center', minWidth: '7rem' }}>
          <div style={{ fontWeight: 700, fontSize: '1.1rem' }}>JAFG</div>
          <div style={{ fontWeight: 200, fontSize: '0.8rem' }}>Inspection</div>
        </div>
      </Link>

      <div style={{ display: 'flex', gap: '0.75rem', alignItems: 'center' }}>
        <Link style={{ color: 'inherit' }} to='/'>Home</Link>
        <Link style={{ color: 'inherit' }} to='/about'>About</Link>
        <Link style={{ color: 'inherit' }} to='/contact'>Contact</Link>
        {GetEnginePingMenu()}
        {/*<Menu>*/}
        {/*  <MenuButton>Connection</MenuButton>*/}
        {/*  <MenuItems style={{ padding: '1rem', minWidth: '20rem', backgroundColor: '#1b202b',*/}
        {/*      border: '2px solid rgba(255,255,255,0.3)', borderRadius: '6px',*/}
        {/*    }} anchor='bottom end'>*/}
        {/*    <MenuItem>*/}
        {/*      <div style={{ display: 'flex', flexDirection: 'column', alignItems: 'left' }}>*/}
        {/*        <MenuButton>Test Connection</MenuButton>*/}
        {/*        <text>Test the connection by pinging it.</text>*/}
        {/*      </div>*/}
        {/*    </MenuItem>*/}
        {/*  </MenuItems>*/}
        {/*</Menu>*/}
      </div>
    </nav>
  );
}
