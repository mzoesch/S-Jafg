import './App.css'
import { Routes, Route } from 'react-router-dom'
import Navbar from "./components/Navbar.tsx";
import HomePage from './pages/Home.tsx'
import NotFound from "./pages/NotFound.tsx";

export default function App() {
  return (
    <div style={{ minHeight: '100vh', width: '100%', }}>
      <Navbar />
      <div style={{ height: '0rem' }}></div>
      <Routes>
        <Route path="/" element={<HomePage />} />
        <Route path="*" element={<NotFound />} />
      </Routes>
    </div>
  )
}