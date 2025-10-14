import { Link } from 'react-router-dom'

export default function NotFound() {
  return (
    <div style={{ padding: 24 }}>
      <h1>404 — Page not found</h1>
      <p>The page you are looking for does not exist.</p>
      <p>
        <Link to="/">Go back home</Link>
      </p>
    </div>
  )
}
