import React from 'react';

interface VirtualPlantProps {
  wellnessScore: number;
}

export const VirtualPlant: React.FC<VirtualPlantProps> = ({ wellnessScore }) => {
  // Déterminer l'état visuel en fonction du score
  let leafColor = "#4ADE80"; // Vert défaut
  let leafDarkColor = "#16A34A";
  let wiltFactor = 0;
  let faceExpression = "happy"; // happy, neutral, sad, ecstatic

  if (wellnessScore >= 100) {
    // 100%: Ecstatic / Parfait
    leafColor = "#22c55e"; // Super vert (Emerald-500)
    leafDarkColor = "#15803d";
    wiltFactor = -5; // Un peu relevé (fier)
    faceExpression = "ecstatic";
  } else if (wellnessScore >= 75) {
    // 75%: Content
    leafColor = "#4ADE80"; // Vert normal
    leafDarkColor = "#16A34A";
    wiltFactor = 0;
    faceExpression = "happy";
  } else if (wellnessScore >= 50) {
    // 50%: Moyen
    leafColor = "#FACC15"; // Jaune
    leafDarkColor = "#A16207";
    wiltFactor = 10;
    faceExpression = "neutral";
  } else {
    // 25% ou moins: Mauvais
    leafColor = "#EF4444"; // Rouge/Marron
    leafDarkColor = "#991B1B";
    wiltFactor = 25; // Très affaissé
    faceExpression = "sad";
  }

  return (
    <div className="relative w-48 h-48 flex items-end justify-center drop-shadow-xl filter transition-all duration-1000 ease-in-out">


      <svg viewBox="0 0 200 200" className="w-full h-full overflow-visible">
        {/* POT */}
        <g transform="translate(60, 140)">
          <path d="M0 0 L80 0 L70 50 L10 50 Z" fill="#D97706" />
          <path d="M-5 0 L85 0 L85 10 L-5 10 Z" fill="#B45309" />
          {/* VISAGE */}
          <circle cx="30" cy="25" r="3" fill="#502810" />
          <circle cx="50" cy="25" r="3" fill="#502810" />

          {/* Bouche dynamique */}
          {faceExpression === "ecstatic" && (
            <path d="M25 35 Q40 50 55 35" stroke="#502810" strokeWidth="3" fill="none" strokeLinecap="round" />
          )}
          {faceExpression === "happy" && (
            <path d="M30 35 Q40 45 50 35" stroke="#502810" strokeWidth="3" fill="none" strokeLinecap="round" />
          )}
          {faceExpression === "neutral" && (
            <line x1="30" y1="40" x2="50" y2="40" stroke="#502810" strokeWidth="3" strokeLinecap="round" />
          )}
          {faceExpression === "sad" && (
            <path d="M30 45 Q40 35 50 45" stroke="#502810" strokeWidth="3" fill="none" strokeLinecap="round" />
          )}
        </g>

        {/* TIGE & FEUILLES */}
        <g transform={`translate(100, 140) rotate(${wiltFactor})`} className="transition-transform duration-1000">
          {/* Tige centrale */}
          <path d="M0 0 Q5 -40 0 -90" stroke="#166534" strokeWidth="6" fill="none" />

          {/* Feuille Gauche */}
          <g transform={`translate(0, -30) rotate(${-45 + wiltFactor})`} className="transition-transform duration-1000">
            <ellipse cx="-20" cy="0" rx="25" ry="10" fill={leafColor} stroke={leafDarkColor} strokeWidth="2" />
            <path d="M0 0 L-40 0" stroke={leafDarkColor} strokeWidth="1" />
          </g>

          {/* Feuille Droite */}
          <g transform={`translate(2, -50) rotate(${45 + wiltFactor})`} className="transition-transform duration-1000">
            <ellipse cx="20" cy="0" rx="25" ry="10" fill={leafColor} stroke={leafDarkColor} strokeWidth="2" />
            <path d="M0 0 L40 0" stroke={leafDarkColor} strokeWidth="1" />
          </g>

          {/* Feuille Haut */}
          <g transform={`translate(0, -90) rotate(${0 + wiltFactor})`} className="transition-transform duration-1000">
            <ellipse cx="0" cy="-20" rx="12" ry="25" fill={leafColor} stroke={leafDarkColor} strokeWidth="2" />
            <path d="M0 0 L0 -40" stroke={leafDarkColor} strokeWidth="1" />
          </g>
        </g>
      </svg>
    </div>
  );
};