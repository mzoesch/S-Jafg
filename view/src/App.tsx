import './App.css'
import { Routes, Route } from 'react-router-dom'
import Navbar from "./components/Navbar.tsx";
import HomePage from './pages/Home.tsx'
import NotFound from "./pages/NotFound.tsx";
import ConsolePage from "./pages/Console.tsx";

export default function App() {
  return (
    <div className='min-h-screen w-screen flex flex-col'>
      <Navbar />
      <main className='flex flex-grow'>
        <Routes>
          <Route path='/' element={<HomePage />} />
          <Route path='*' element={<NotFound />} />
          <Route path='/console' element={<ConsolePage />} />
        </Routes>
      </main>
    </div>
  )
}