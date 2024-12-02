/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   three.js                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdelvign <gdelvign@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 21:25:52 by gdelvign          #+#    #+#             */
/*   Updated: 2024/11/20 21:25:55 by gdelvign         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

import * as THREE from "three";
import { OrbitControls } from 'three/addons/controls/OrbitControls.js';

const scene = new THREE.Scene();
scene.background = new THREE.Color( 0xe5e7eb);
const camera = new THREE.PerspectiveCamera(
  75,
  window.innerWidth / window.innerHeight,
  0.1,
  1000
);

const renderer = new THREE.WebGLRenderer();
renderer.setSize(window.innerWidth / 2, window.innerHeight / 2);
const container = document.getElementById("content-container");
container.appendChild(renderer.domElement);

const geometry = new THREE.BoxGeometry(1, 1, 1);
const material = new THREE.MeshBasicMaterial({ color: 0x0d9488 });
const cube = new THREE.Mesh(geometry, material);
scene.add(cube);

const controls = new OrbitControls( camera, renderer.domElement );
controls.update();

camera.position.z = 2;

function animate() {
  cube.rotation.x += 0.01;
  cube.rotation.y += 0.01;
  renderer.render(scene, camera);
}
renderer.setAnimationLoop(animate);




