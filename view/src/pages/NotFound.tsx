// Copyright mzoesch. All rights reserved.

import {Link} from 'react-router-dom';

export default function NotFound() {
  const route = window.location.pathname

  return (
    <div className='flex flex-col flex-1 justify-between h-full py-50'>
      <div>
        <h1 className='font-extrabold'>Whooooops</h1>
      </div>
      <div>
        <h2>It looks like <span className='font-mono font-bold text-indigo-400'>{route}</span>{' '}
          went missing in the matrix 😖</h2>
      </div>
      <div className='flex justify-between gap-3 mb-4 ml-30 mr-30'>
        <div>
          Learn more about{' '}
          <span>
              <Link to='https://en.wikipedia.org/wiki/HTTP_404' target='_blank'>
                error 404
              </Link>
            </span>
        </div>
        <div>
          <Link to='https://www.youtube.com/watch?v=dQw4w9WgXcQ'>
            Take me back, where I belong
          </Link>
        </div>
      </div>
    </div>
  )
}
